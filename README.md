# 总体设计文档

版本：1.0  
作者：曹家茗  
日期：2026-06-17

---

## 一、项目名称 / 文档作者
- 项目名称：Ticket-System-2026
- 文档作者：曹家茗

---

## 二、程序功能概述
Ticket-System-2026 是基于命令行的火车票订票系统（单进程、单线程），模拟 12306 核心业务。主要功能：

- 用户管理：注册（add_user）、登录（login）、登出（logout）、查询个人信息（query_profile）、修改个人信息（modify_profile）
- 车次管理：添加车次（add_train）、删除车次（delete_train）、发布车次（release_train）、查询车次（query_train）
- 车票业务：查询直达车票（query_ticket）、查询换乘方案（query_transfer）、购票（buy_ticket）、候补购票（buy_ticket -q true）、退票（refund_ticket）、查询订单（query_order）
- 持久化：以若干固定格式的二进制文件保存数据（见"数据库设计"），并支持多次重启
- 基本权限控制：用户权限（0~10 整数），用于区分普通用户与管理员

程序启动时读取/初始化二进制数据文件（total、accountdata、traindata、trainIDdata、orderdata 及各 B+ 树索引文件）；程序退出时将各管理器状态写回 total 并关闭文件。

---

## 三、主体逻辑说明
程序采用 "Parser（输入解析）→ Program（应用协调）→ 各 Manager（功能实现）→ 二进制文件持久化" 的流程：

1. 启动：
   - 程序打开/创建 total（用于记录各子管理器的元信息）。
   - 各 Manager（AccountManager、TrainManager、OrderManager）尝试从 total 中读取自身记录信息（通过 changeXXXCount）并打开/创建各自专属文件（accountdata、trainIDdata、traindata、orderdata）及 B+ 树索引文件。

2. 命令循环：
   - 用户输入一行命令由 Parser::parseLine 分发到各种 parse 函数（每个 parse 函数代表一种命令）并解析（语法与参数校验），解析通过后调用 Program 中相应方法（如 AddUser, Login, Logout, QueryProfile, AddTrain, ReleaseTrain, QueryTicket, BuyTicket 等）。
   - Program 调用 AccountManager、TrainManager、OrderManager 等实现具体操作，并通过 loginrecorder 管理登录状态，通过 pendingorders 管理候补队列。

3. 退出：
   - Program 析构时将各 Manager 的计数信息及候补队列写回 total，然后关闭所有文件。

关键责任分配：
- Parser：把一行文本解析为命令与参数（包含严格的格式检查）。
- Program：校验权限/会话（loginrecorder 管理），协调操作并调用 Manager 完成业务。
- AccountManager：用户记录增删改查，维护账号索引（account_bpt）（长效）。
- TrainManager：车次记录读写、索引（trainID_bpt、station_bpt、stationpair_bpt）、余票管理、车票查询与换乘查询（长效）。
- OrderManager：订单记录的读写、用户订单索引（userorder_bpt）（长效）。
- 持久化采用固定宽度二进制记录文件，读写由各 Manager 直接操作文件流完成。

错误与异常处理：使用 TicketSystemError 异常类型（utils/Error.hpp），在参数校验或状态不满足时抛出异常。

---

## 四、代码文件结构

```
.
├── CMakeLists.txt
├── README.md
├── include/
│   ├── Program.hpp                     // 应用协调（会话/命令调用/文件总控）
│   ├── Parser.hpp                      // 命令解析
│   ├── AccountManager.hpp              // 账号管理，账号文件 accountdata
│   ├── TrainManager.hpp                // 车次 & 车票管理，车次文件 trainIDdata / traindata
│   ├── OrderManager.hpp                // 订单管理，订单文件 orderdata
│   ├── STLite/
│   │   ├── BPT.hpp                     // B+ 树模板（索引核心）
│   │   ├── vector.hpp                  // 动态数组
│   │   ├── map.hpp                     // 红黑树映射
│   │   ├── set.hpp                     // 红黑树集合
│   │   ├── deque.hpp                   // 双端队列（候补队列）
│   │   ├── pair.hpp                    // 键值对
│   │   └── string.hpp                  // 定长字符串模板
│   └── utils/
│       ├── Error.hpp                   // 自定义异常类型
│       └── constants.hpp                   // 工具函数 & 常量（时间计算、输出格式化、sizeof 常量）
└── src/
    ├── TicketSystem.cpp                // 入口 (main)
    ├── Program.cpp                     // 应用协调（会话/命令调用/文件总控）
    ├── Parser.cpp                      // 命令解析
    ├── AccountManager.cpp              // 账号管理实现
    ├── TrainManager.cpp                // 车次 & 车票实现
    └── OrderManager.cpp                // 订单管理实现
```

运行时生成的数据文件：
```
.
├── total                              // 各管理器元信息（计数 & 候补队列）
├── accountdata                        // 用户数据
│   ├── accountbasic                   // B+ 树节点数据
│   └── accountindex                   // B+ 树索引元信息
├── trainIDdata                        // 车次基础信息（TrainBasic / TrainView）
│   ├── trainIDbasic                   // B+ 树节点数据
│   └── trainIDindex                   // B+ 树索引元信息
├── traindata                          // 每日列车实例（含各区间余票）
├── orderdata                          // 订单数据
│   ├── orderdatabasic                 // B+ 树节点数据
│   └── orderdataindex                 // B+ 树索引元信息
├── stationbasic + stationindex        // 站名 → (车次, 站序号) B+ 树索引
└── stationpairbasic + stationpairindex // (出发站, 到达站) → (车次, (原点, 终点)) B+ 树索引
```

main 与类关系说明：
- 程序的启动入口在 TicketSystem.cpp（main），构造 Program 对象（Program::Program），Program 在构造中打开 total 并初始化 AccountManager、TrainManager、OrderManager。
- Program 持有：
  - AccountManager accountmanager;                              // 处理用户相关内容
  - TrainManager trainmanager;                                  // 处理车次 & 车票相关内容
  - OrderManager ordermanager;                                  // 处理订单相关内容
  - Parser parser;                                              // 解析语句
  - sjtu::map<sjtu::string<20>, int> loginrecorder;              // 当前登录用户映射（username → 权限）
  - sjtu::deque<int> pendingorders;                              // 候补订单队列（Order 索引）
  - std::fstream total_file;                                     // total 元信息文件
- Parser 负责解析后回调 Program 的方法。
- 各 Manager 直接操作对应的二进制文件保存数据。

简化调用关系（示意）：
main → Program  
Program → Parser（解析输入 → 回调 Program）  
Program → AccountManager / TrainManager / OrderManager  
Managers → 对应二进制文件 + B+ 树索引文件进行读写（持久化）

---

## 五、功能设计
模块划分（对应代码文件）：

### CLI 层
- Parser：命令解析、参数校验（严格的格式检查规则）
- Program：会话管理（loginrecorder）、命令分发、程序生命周期

### 业务/持久化层（Managers）

#### AccountManager
- 功能：添加/查找/修改用户（addAccount, getAccount, writeAccount, getIndex）
- 索引：account_bpt（B+ 树，username → 文件内索引）
- 关键结构体：
  - Account：用户基础信息
- 持久化文件：
  - accountdata（固定宽度记录用户信息：password + name + mailAddr + privilege）
  - accountbasic + accountindex（B+ 树索引文件）

#### TrainManager
- 功能：
  - 车次管理：添加/删除/发布/查询车次（addTrain, deleteTrain, releaseTrain, queryTrain）
  - 车票查询：queryTicket、queryTransfer——按时间/价格排序
  - 余票管理：getSeat（查询区间最小余票）、writeTrain（更新余票）
- 索引：
  - trainID_bpt（B+ 树，trainID → 车次索引）
  - station_bpt（B+ 树，站名 → (车次索引, 站序号)）
  - stationpair_bpt（B+ 树，(出发站, 到达站) → (车次索引, (出发站序号, 到达站序号))）
- 关键结构体：
  - TrainBasic：车次基础信息
  - TrainView：TrainBasic 去掉 stations 数组和类型的轻量版本（用于 queryTicket 快速读取）
  - Train：每日列车实例（含 99 个区间的余票数组）
  - Ticket：查询结果结构（trainID, 出发/到达时间, 价格, 余票）
  - Transfer：换乘结果结构（两段车次信息）
- 排序比较器：compareTicketTime / compareTicketCost / compareTransferTime / compareTransferCost
- 持久化文件：
  - trainIDdata（固定宽度记录 TrainBasic）
  - traindata（固定宽度记录 Train，每个 trainNum 天一个实例）
  - trainIDbasic + trainIDindex（B+ 树索引）
  - stationbasic + stationindex（B+ 树索引）
  - stationpairbasic + stationpairindex（B+ 树索引）

#### OrderManager
- 功能：添加订单（addPendingOrder / addSuccessOrder）、查询订单（getOrder / getOrderView）、修改订单状态（writeOrderState）
- 索引：userorder_bpt（B+ 树，username → 订单索引列表）
- 关键结构体：
  - Order：完整订单（state, 出发/到达时间, 价格, 数量, 起止站, trainID, station1, station2）
  - OrderView：订单轻量视图（价格, 数量, 起止站, train_index，用于候补处理）
- 持久化文件：
  - orderdata（固定宽度记录 Order）
  - orderdatabasic + orderdataindex（B+ 树索引）

### Utils
- constants.hpp：时间计算（culculateTime）、时间格式化输出（printTime）、数值格式化（printNum）、月历常量、sizeof 常量
- Error.hpp：自定义异常类型 TicketSystemError 及派生类

### 功能结构图
```
Program（运行核心，命令处理）
├─ Parser（解析命令 → 回调 Program）
├─ AccountManager（账号 CRUD, B+ 树索引）
├─ TrainManager（车次 CRUD/索引/余票/查询/换乘）
└─ OrderManager（订单 CRUD/索引）
```

主要命令详见 `management_system.md`。

---

## 六、数据库设计
采用若干固定格式的二进制文件（按 Manager 命名）。所有文件以固定宽度字段顺序写入，程序通过偏移量读/写记录。

总体元信息文件：total
- total 中保存各管理器的计数（account_count, trainID_count, train_count, order_count）及候补队列（pendingorders）。
- Program 在构造时读取这些信息来恢复各 Manager 状态，并在析构时写回。

具体文件及记录结构：

### 1) accountdata（AccountManager）
- 单条记录结构：
  - password: sjtu::string<30>（密码）
  - name: sjtu::string<15>（真实姓名）
  - mailAddr: sjtu::string<30>（邮箱）
  - privilege: int（权限）
- 字节偏移量 = account_size × index
- 索引：account_bpt（B+ 树，username → index）

### 2) trainIDdata（TrainManager）
- 单条记录固定字段（TrainBasic 结构体）：
  - state: int（0: 未发布, 1: 已发布, 2: 已删除）
  - headtrainindex: int（该车次在 traindata 中的起始索引）
  - trainNum: int（售卖天数）
  - leavingTimes[100]: int（各站出发时间，以分钟计，0 站为始发站）
  - arrivingTimes[100]: int（各站到达时间）
  - sum_prices[100]: int（累计票价）
  - stationNum: int（车站数量）
  - seatNum: int（座位数）
  - trainID: sjtu::string<20>（车次 ID）
  - stations[100]: sjtu::string<30>（站名数组）
  - type: char（列车类型）
- 字节偏移量 = sizeofTrainBasic × index
- 索引：trainID_bpt（trainID → index）

### 3) traindata（TrainManager）
- 单条记录固定字段（Train 结构体）：
  - seats[99]: int（各区间余票，第 i 项表示第 i 站到第 i+1 站的余票）
- 每个车次的每一天有一个 Train 实例，共 trainNum 个
- 字节偏移量 = sizeofTrain × index

### 4) orderdata（OrderManager）
- 单条记录固定字段（Order 结构体）：
  - state: int（0: success, 1: pending, 2: refunded）
  - leavingtime: int（出发绝对时间）
  - arrivingtime: int（到达绝对时间）
  - price: int（票价）
  - num: int（购票数量）
  - origin: int（出发站序号）
  - destination: int（到达站序号）
  - train_index: int（对应 Train 在 traindata 中的索引）
  - trainID: sjtu::string<20>（车次 ID）
  - station1: sjtu::string<30>（出发站名）
  - station2: sjtu::string<30>（到达站名）
- 字节偏移量 = sizeofOrder × index
- 索引：userorder_bpt（username → index 列表）

### 5) B+ 树索引文件
每个 B+ 树由一个 basic 文件（存储节点数据）和一个 index 文件（存储根节点索引等元信息）组成：
- accountbasic + accountindex（username → account 索引）
- trainIDbasic + trainIDindex（trainID → trainID 索引）
- stationbasic + stationindex（站名 → (trainID 索引, 站序号)）
- stationpairbasic + stationpairindex（(站名, 站名) → (trainID 索引, (原点, 终点))）
- orderdatabasic + orderdataindex（username → order 索引列表）

---

## 七、类 / 结构体设计

### 1) Account 结构体
```cpp
struct Account {
    sjtu::string<30> password;
    sjtu::string<15> name;
    sjtu::string<30> mailAddr;
    int privilege;
};
```

### 2) TrainBasic 结构体
```cpp
struct TrainBasic {
    int state;           // 0: 未发布, 1: 已发布, 2: 已删除
    int headtrainindex;  // 在 traindata 中的起始索引
    int trainNum;        // 售卖天数
    int leavingTimes[100], arrivingTimes[100], sum_prices[100];
    int stationNum, seatNum;
    sjtu::string<20> trainID;
    sjtu::string<30> stations[100];
    char type;
};
```

### 3) TrainView 结构体（TrainBasic 去掉 stations 和 type 的轻量版）
```cpp
struct TrainView {
    int state, headtrainindex, trainNum;
    int leavingTimes[100], arrivingTimes[100], sum_prices[100];
    int stationNum, seatNum;
    sjtu::string<20> trainID;
};
```

### 4) Train 结构体
```cpp
struct Train {
    int seats[99];  // 各区间余票
};
```

### 5) Ticket 结构体
```cpp
struct Ticket {
    sjtu::string<20> trainID;
    int leavingtime, arrivingtime, price, seat;
};
```

### 6) Transfer 结构体
```cpp
struct Transfer {
    sjtu::string<20> train1ID, train2ID;
    sjtu::string<30> transferstation;
    int leavingtime1, arrivingtime1, price1, seat1;
    int leavingtime2, arrivingtime2, price2, seat2;
};
```

### 7) Order / OrderView 结构体
```cpp
struct Order {
    int state;  // 0: success, 1: pending, 2: refunded
    int leavingtime, arrivingtime, price, num;
    int origin, destination, train_index;
    sjtu::string<20> trainID;
    sjtu::string<30> station1, station2;
};

struct OrderView {  // 轻量视图，用于候补处理
    int price, num;
    int origin, destination, train_index;
};
```

### 8) BplusTree 模板类（B+ 树，索引核心）
```cpp
template<class T, class U>
class BplusTree {
  private:
    static constexpr int desired_fanout = 64;
    static constexpr int sizeofNode = 4096;  // 节点大小对齐到 4KB
    static constexpr int cache_size = 256;  // LRU 缓存节点数

    // 内部节点
    class internal_Node : public Node {
        T keys[Isize + 1];
        U values[Isize + 1];
        int nodes_index[Isize + 2];
    };
    // 叶子节点
    class leaf_Node : public Node {
        T keys[Lsize + 1];
        U values[Lsize + 1];
        int next;
    };

    std::fstream basic_file, index_file;  // 节点数据文件 + 索引元信息文件
    int root_index;  // 根节点索引
    // LRU 缓存相关成员
    // ...

  public:
    void insert(const T &key, const U &value);  // 插入
    void remove(const T &key, const U &value);  // 删除
    sjtu::vector<U> find(const T &key);          // 查找
    void clear();                                 // 清空
};
```

### 9) sjtu::string\<N\> 模板
```cpp
template<int strlength>
struct string {
    char data[strlength];
    int len;
    // 构造函数、length()、empty()、operator[]、c_str()、push_back()、clear()
    // 比较运算符：<, >, ==, !=, <=, >=
    // 流输出：operator<<
};
```

### 10) Program 类
```cpp
class Program {
  private:
    std::string total_filename;
    std::fstream total_file;
    AccountManager accountmanager;
    TrainManager trainmanager;
    OrderManager ordermanager;
    Parser parser;
    sjtu::map<sjtu::string<20>, int> loginrecorder;  // 登录用户 → 权限
    sjtu::deque<int> pendingorders;                    // 候补订单队列
    bool programEnd_;

  public:
    Program();   // 构造：打开 total 并初始化各 Manager
    ~Program();  // 析构：将计数 & 候补队列写回 total
    void execute(const std::string &line);
    bool programRun();
    void programEnd();

    // 用户管理
    void AddUser(...);
    void Login(...);
    void Logout(...);
    void QueryProfile(...);
    void ModifyProfile(...);

    // 车次管理
    void AddTrain(...);
    void DeleteTrain(...);
    void ReleaseTrain(...);
    void QueryTrain(...);

    // 车票业务
    void QueryTicket(...);
    void QueryTransfer(...);
    void BuyTicket(...);
    void QueryOrder(...);
    void RefundTicket(...);

    void Clean();
    void Exit();
};
```

### 11) Parser 类
```cpp
class Parser {
  public:
    void parseLine(const std::string &line, Program *program);
  private:
    void parseAddUser(...);
    void parseLogin(...);
    void parseLogout(...);
    void parseQueryProfile(...);
    void parseModifyProfile(...);
    void parseAddTrain(...);
    void parseDeleteTrain(...);
    void parseReleaseTrain(...);
    void parseQueryTrain(...);
    void parseQueryTicket(...);
    void parseQueryTransfer(...);
    void parseBuyTicket(...);
    void parseQueryOrder(...);
    void parseRefundTicket(...);
    void parseClean(...);
    void parseExit(...);
};
```

### 12) AccountManager / TrainManager / OrderManager
详见各头文件，核心设计要点：
- 每个 Manager 持有对应的 `std::fstream` 文件句柄和 B+ 树索引
- 通过 `changeXXXCount` / `getXXXCount` 与 Program 交互元信息
- 通过 `clean()` 支持清空所有数据
- B+ 树自动处理文件持久化，节点缓存（LRU）减少磁盘 I/O

---

## 八、构建与运行

### 编译
```bash
mkdir build && cd build
cmake ..
make
```

### 运行
```bash
./code < input.in > output.out
```

### 输入格式
```
[<timestamp>] <command> -<key1> <arg1> -<key2> <arg2> ...
```
每条指令前有时间戳，输出首行需回显 `[<timestamp>]`。

---

## 九、补充说明
- B+ 树节点大小固定为 4096 字节（4KB），对齐磁盘页，最大化 I/O 效率
- 节点池使用 LRU 缓存（256 个节点），减少频繁磁盘读写
- 自实现容器（STLite）完全替代 STL，满足课程限制要求
- 所有时间以分钟为单位存储（自 2026-06-01 00:00 起），输出时按 `mm-dd hh:mm` 格式化
- 可以通过修改 constants.hpp 中的常量控制整个项目结构