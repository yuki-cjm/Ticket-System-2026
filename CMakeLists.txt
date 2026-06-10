cmake_minimum_required(VERSION 3.10)
project(TicketSystem)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加调试标志
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# 包含目录
include_directories(include)

# 源文件
set(SOURCES
    src/AccountManager.cpp
    src/TrainManager.cpp
    src/Parser.cpp
    src/Program.cpp
    src/TicketSystem.cpp
)

# 创建可执行文件
add_executable(code ${SOURCES})