#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

constexpr int sizeofint = sizeof(int);
constexpr int sizeofNode = 4096;
constexpr int cache_size = 1024;

template<int strlength>
class BplusTree {
  private:
    static constexpr int Isize = (sizeofNode - 1 - sizeofint * 3) / (strlength + 1 + sizeofint * 2);
    static constexpr int Lsize = (sizeofNode - 1 - sizeofint * 3) / (strlength + 1 + sizeofint);
    static constexpr int half_Isize = Isize >> 1;
    static constexpr int half_Lsize = Lsize >> 1;
  private:
    class Node {
      public:
        bool type;
        int index, count;

        virtual void read(const char *buf) = 0;
        virtual void serialize(char *buf) = 0;
        virtual void print() = 0;
        virtual ~Node() = default;
    };

    class internal_Node : public Node {
      public:
        char keys[Isize + 1][strlength + 1];
        int values[Isize + 1];
        int nodes_index[Isize + 2];

        internal_Node() {
            this->type = false;
            this->index = -1;
            this->count = 0;
        }
        ~internal_Node() = default;

        void read(const char *buf) override {
            const char *p = buf + 1;
            std::memcpy(&this->count, p, sizeofint); p += sizeofint;
            std::memcpy(&this->index, p, sizeofint); p += sizeofint;
            if (this->count > 0) {
                std::memcpy(keys, p, this->count * (strlength + 1));
                p += this->count * (strlength + 1);
                for (int i = 0; i < this->count; i++) 
                    keys[i][strlength] = '\0';
                std::memcpy(values, p, this->count * sizeofint);
                p += this->count * sizeofint;
            }
            std::memcpy(nodes_index, p, (this->count + 1) * sizeofint);
        }

        void serialize(char *buf) override {
            char *p = buf;
            std::memcpy(p, &this->type, 1);   p += 1;
            std::memcpy(p, &this->count, sizeofint);  p += sizeofint;
            std::memcpy(p, &this->index, sizeofint);  p += sizeofint;
            for (int i = 0; i < this->count; ++i) {
                std::memcpy(p, keys[i], strlength + 1);
                p += strlength + 1;
            }
            for (int i = 0; i < this->count; i++) {
                std::memcpy(p, &values[i], sizeofint);
                p += sizeofint;
            }
            for (int i = 0; i <= this->count; i++) {
                std::memcpy(p, &nodes_index[i], sizeofint);
                p += sizeofint;
            }
        }

        void print() override {
            std::cout << "type : internal_Node\n";
            std::cout << "index : " << this->index << '\n';
            std::cout << "count : " << this->count << '\n';
            std::cout << nodes_index[0] << ' ';
            for (int i = 0; i < this->count; i++) {
                std::cout << '(' << keys[i] << ", " << values[i] << ") " << nodes_index[i + 1] << ' ';
            }
            std::cout << '\n';
            std::cout << std::endl;
        }
    };

    class leaf_Node : public Node {
      public:
        int next_index;
        char keys[Lsize][strlength + 1];
        int values[Lsize];

        leaf_Node() {
            this->type = true;
            this->index = -1;
            this->count = 0;
        }
        ~leaf_Node() = default;

        void read(const char *buf) override {
            const char *p = buf + 1;
            std::memcpy(&this->count, p, sizeofint); p += sizeofint;
            std::memcpy(&this->index, p, sizeofint); p += sizeofint;
            std::memcpy(&next_index, p, sizeofint); p += sizeofint;
            for (int i = 0; i < this->count; i++) {
                std::memcpy(keys[i], p, strlength + 1);
                keys[i][strlength] = '\0';
                p += strlength + 1;
                std::memcpy(&values[i], p, sizeofint);
                p += sizeofint;
            }
        }

        void serialize(char *buf) override {
            char *p = buf;
            std::memcpy(p, &this->type, 1);p += 1;
            std::memcpy(p, &this->count, sizeofint); p += sizeofint;
            std::memcpy(p, &this->index, sizeofint); p += sizeofint;
            std::memcpy(p, &next_index, sizeofint); p += sizeofint;
            for (int i = 0; i < this->count; i++) {
                std::memcpy(p, keys[i], strlength + 1);
                p += strlength + 1;
                std::memcpy(p, &values[i], sizeofint); 
                p += sizeofint;
            }
        }

        void print() override {
            std::cout << "type : leaf_Node\n";
            std::cout << "index : " << this->index << '\n';
            std::cout << "next_index : " << next_index << '\n';
            std::cout << "count : " << this->count << '\n';
            for (int i = 0; i < this->count; i++) {
                std::cout << '(' << keys[i] << ", " << values[i] << ")  ";
            }
            std::cout << '\n';
            std::cout << std::endl;
        }
    };


    static int keycmp(const char a[strlength + 1], const std::string &b) {
        return std::strncmp(a, b.c_str(), strlength + 1);
    }

    static int keycmp(const char a[strlength + 1], const char b[strlength + 1]) {
        return std::strncmp(a, b, strlength + 1);
    }

    static void set_key(char dest[strlength + 1], const std::string &src) {
        std::strncpy(dest, src.c_str(), strlength);
        dest[strlength] = '\0';
    }

    static void set_key(char dest[strlength + 1], const char src[strlength + 1]) {
        std::memcpy(dest, src, strlength + 1);
    }


    std::string basic_filename, data_filename;
    std::fstream file;
    int Nodecount;
    int root_index, deleted_count;

    // Memory 节点拥有其 node，并在析构时级联释放整条 father 链
    struct Memory {
        Memory* father;
        Node* node;
        int index;

        Memory(Memory* father_, Node* node_, int index_) : father(father_), node(node_), index(index_) {};
        ~Memory() {
            delete node;
            delete father;
        }
    };
    

    struct deleted_Node {
        int index;
        deleted_Node* next;

        deleted_Node(int index_, deleted_Node* next_) : index(index_), next(next_) {}
    };
    deleted_Node* deleted_node;

    void add_deleted_Node(int index) {
        deleted_Node *new_deleted_node = new deleted_Node(index, deleted_node);
        deleted_node = new_deleted_node;
        deleted_count++;
        clearNode(index);
    }

    // 分配一个空的Node的index
    int EmptyNodeIndex() {
        int ans;
        if (!deleted_count) {
            ans = Nodecount;
            Nodecount++;
        } else {
            ans = deleted_node->index;
            deleted_Node *temp = deleted_node;
            deleted_node = deleted_node->next;
            delete temp;
            deleted_count--;
        }
        return ans;
    }


    struct Cache {
        int node_index = -1;
        char buf[sizeofNode];
    } cache[cache_size];

    // 返回一个新分配的 node，所有权归调用者，调用者负责 delete
    Node* getNode(int index) {
        int p = index % cache_size;
        if (cache[p].node_index != index) {
            file.seekg(index * sizeofNode);
            file.read(cache[p].buf, sizeofNode);
            cache[p].node_index = index;
        }
        const char *buf = cache[p].buf;
        Node *node;
        if (buf[0]) {
            leaf_Node *leaf = new leaf_Node();
            leaf->read(buf);
            node = leaf;
        } else {
            internal_Node *inner = new internal_Node();
            inner->read(buf);
            node = inner;
        }
        return node;
    }

    // 写入磁盘并同步更新缓存字节
    void writeNode(Node *node) {
        char buf[sizeofNode] = {};
        node->serialize(buf);
        file.seekp(node->index * sizeofNode);
        file.write(buf, sizeofNode);
        int p = node->index % cache_size;
        cache[p].node_index = node->index;
        std::memcpy(cache[p].buf, buf, sizeofNode);
    }

    void clearNode(int index) {
        int p = index % cache_size;
        if (cache[p].node_index == index) {
            cache[p].node_index = -1;
        }
    }


    std::pair<Memory*, bool> search(const std::string &key, int value) { // (a, b) -> b
        if (root_index == -1) {
            return {nullptr, false};
        }

        int index = root_index;
        Memory *last = nullptr, *memory;
        while (true) {
            Node *temp = getNode(index);
            if (temp->type) {
                leaf_Node *node = static_cast<leaf_Node*>(temp);
                int l = 0, r = node->count, m;
                while (l != r) {
                    m = (l + r) >> 1;
                    int cmp = keycmp(node->keys[m], key);
                    if (cmp > 0 || cmp == 0 && node->values[m] > value)
                        r = m;
                    else if (cmp < 0 || cmp == 0 && node->values[m] < value)
                        l = m + 1;
                    else {
                        memory = new Memory(last, node, m);
                        return {memory, true};
                    }
                }
                memory = new Memory(last, node, l);
                return {memory, false};
            } else {
                internal_Node *node = static_cast<internal_Node*>(temp);
                int l = 0, r = node->count, m;
                while (l != r) {
                    m = (l + r) >> 1;
                    int cmp = keycmp(node->keys[m], key);
                    if (cmp > 0 || cmp == 0 && node->values[m] > value)
                        r = m;
                    else
                        l = m + 1;
                }
                memory = new Memory(last, node, l);
                last = memory;
                index = node->nodes_index[l];
            }
        }
    }
  
    // 向内部节点插入key & value
    void insert_internal(Memory *memory, const char key[strlength + 1], int value, int child_index) {
        internal_Node *node = static_cast<internal_Node*>(memory->node);
        int node_index = node->index;

        int l = 0, r = node->count, m;
        while (l < r) {
            m = (l + r) >> 1;
            int cmp = keycmp(node->keys[m], key);
            if (cmp > 0 || (cmp == 0 && node->values[m] > value))
                r = m;
            else
                l = m + 1;
        }
        int pos = l;

        std::memmove(node->keys[pos + 1], node->keys[pos], (node->count - pos) * (strlength + 1));
        std::memmove(node->values + pos + 1, node->values + pos, (node->count - pos) * sizeofint);
        set_key(node->keys[pos], key);
        node->values[pos] = value;
        
        std::memmove(node->nodes_index + pos + 2, node->nodes_index + pos + 1, (node->count - pos) * sizeofint);
        node->nodes_index[pos + 1] = child_index;
        node->count++;

        if (node->count <= Isize) {
            writeNode(node);
        } else {
            internal_Node *new_node = new internal_Node();
            new_node->index = EmptyNodeIndex();
            new_node->count = Isize - half_Isize;
            node->count = half_Isize;
            std::memcpy(new_node->keys, node->keys[half_Isize + 1], new_node->count * (strlength + 1));
            std::memcpy(new_node->values, node->values + half_Isize + 1, new_node->count * sizeofint);
            std::memcpy(new_node->nodes_index, node->nodes_index + half_Isize + 1, (new_node->count + 1) * sizeofint);

            if (node->index == root_index) {
                internal_Node *new_root = new internal_Node();
                set_key(new_root->keys[0], node->keys[half_Isize]);
                new_root->values[0] = node->values[half_Isize];
                new_root->nodes_index[0] = node_index;
                new_root->nodes_index[1] = new_node->index;
                new_root->count = 1;
                new_root->index = EmptyNodeIndex();
                writeNode(new_root);
                writeNode(node);
                writeNode(new_node);
                root_index = new_root->index;
                delete new_root;
                delete new_node;
            } else {
                writeNode(node);
                writeNode(new_node);
                insert_internal(memory->father, node->keys[half_Isize], node->values[half_Isize], new_node->index);
                delete new_node;
            }
        }
    }

    void fatherRemove(internal_Node *node, int data_index, int node_index) {
        std::memmove(node->keys[data_index], node->keys[data_index + 1], (node->count - 1 - data_index) * (strlength + 1));
        std::memmove(node->values + data_index, node->values + data_index + 1, (node->count - 1 - data_index) * sizeofint);
        std::memmove(node->nodes_index + node_index, node->nodes_index + node_index + 1, (node->count - node_index) * sizeofint);
        node->count--;
    }

    void remove_internal(Memory *memory) {
        internal_Node *node = static_cast<internal_Node*>(memory->node);

        if (node->count >= half_Isize)
            return;

        if (node->index == root_index) {
            if (node->count == 0) {
                root_index = node->nodes_index[0];
                add_deleted_Node(node->index);
            }
            return;
        }

        internal_Node *father = static_cast<internal_Node*>(memory->father->node);
        int index = memory->father->index;
        int left_index = -1, right_index = -1;
        internal_Node *left = nullptr, *right = nullptr;

        if (index > 0) {
            left_index = father->nodes_index[index - 1];
            left = static_cast<internal_Node*>(getNode(left_index));
            if (left->count > half_Isize) {
                std::memmove(node->keys[1], node->keys[0], node->count * (strlength + 1));
                std::memmove(node->values + 1, node->values, node->count * sizeofint);
                std::memmove(node->nodes_index + 1, node->nodes_index, (node->count + 1) * sizeofint);
                set_key(node->keys[0], father->keys[index - 1]);
                node->values[0] = father->values[index - 1];
                node->nodes_index[0] = left->nodes_index[left->count];
                set_key(father->keys[index - 1], left->keys[left->count - 1]);
                father->values[index - 1] = left->values[left->count - 1];
                node->count++;
                left->count--;

                writeNode(node);
                writeNode(left);
                writeNode(father);
                delete left;
                delete right;
                return;
            }
        }
        if (index < father->count) {
            right_index = father->nodes_index[index + 1];
            right = static_cast<internal_Node*>(getNode(right_index));
            if (right->count > half_Isize) {
                set_key(node->keys[node->count], father->keys[index]);
                node->values[node->count] = father->values[index];
                node->nodes_index[node->count + 1] = right->nodes_index[0];
                set_key(father->keys[index], right->keys[0]);
                father->values[index] = right->values[0];
                std::memmove(right->keys[0], right->keys[1], (right->count - 1) * (strlength + 1));
                std::memmove(right->values, right->values + 1, (right->count - 1) * sizeofint);
                std::memmove(right->nodes_index, right->nodes_index + 1, right->count * sizeofint);
                node->count++;
                right->count--;

                writeNode(node);
                writeNode(right);
                writeNode(father);
                delete left;
                delete right;
                return;
            }
        }
        if (index > 0) {
            set_key(left->keys[left->count], father->keys[index - 1]);
            left->values[left->count] = father->values[index - 1];
            left->count++;
            std::memcpy(left->keys + left->count, node->keys, node->count * (strlength + 1));
            std::memcpy(left->values + left->count, node->values, node->count * sizeofint);
            std::memcpy(left->nodes_index + left->count, node->nodes_index, (node->count + 1) * sizeofint);
            left->count += node->count;

            writeNode(left);
            add_deleted_Node(node->index);

            fatherRemove(father, index - 1, index);
            writeNode(father);

            if (father->index == root_index && father->count == 0) {
                root_index = left_index;
                add_deleted_Node(father->index);
            } else if (father->count < half_Isize && father->index != root_index) {
                remove_internal(memory->father);
            }
        } else {
            set_key(node->keys[node->count], father->keys[0]);
            node->values[node->count] = father->values[0];
            node->count++;
            std::memcpy(node->keys + node->count, right->keys, right->count * (strlength + 1));
            std::memcpy(node->values + node->count, right->values, right->count * sizeofint);
            std::memcpy(node->nodes_index + node->count, right->nodes_index, (right->count + 1) * sizeofint);
            node->count += right->count;
            writeNode(node);
            add_deleted_Node(right_index);
            fatherRemove(father, 0, 1);
            writeNode(father);

            if (father->index == root_index && father->count == 0) {
                root_index = node->index;
                add_deleted_Node(father->index);
            } else if (father->count < half_Isize && father->index != root_index) {
                remove_internal(memory->father);
            }
        }
        delete left;
        delete right;
    }

  public:
    BplusTree (const std::string &basic_filename_, const std::string &data_filename_) : basic_filename(basic_filename_), data_filename(data_filename_) {
        file.open(basic_filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!file) {
            file.open(basic_filename, std::ios::out | std::ios::binary);
            file.close();
            Nodecount = 0;
            root_index = -1;
            deleted_count = 0;
            deleted_node = nullptr;
            file.open(data_filename, std::ios::in | std::ios::out | std::ios::binary);
            if (!file) {
                file.open(data_filename, std::ios::out | std::ios::binary);
                file.close();
                file.open(data_filename, std::ios::in | std::ios::out | std::ios::binary);
            }
        } else {
            file.seekg(0);
            if (!file.read(reinterpret_cast<char*>(&Nodecount), sizeofint) ||
                !file.read(reinterpret_cast<char*>(&root_index), sizeofint) ||
                !file.read(reinterpret_cast<char*>(&deleted_count), sizeofint)) {
                Nodecount = 0;
                root_index = -1;
                deleted_count = 0;
                deleted_node = nullptr;
                file.close();
                file.open(data_filename, std::ios::in | std::ios::out | std::ios::binary);
                if (!file) {
                    file.open(data_filename, std::ios::out | std::ios::binary);
                    file.close();
                    file.open(data_filename, std::ios::in | std::ios::out | std::ios::binary);
                }
                return;
            }
            int index;
            deleted_Node* last = nullptr;
            deleted_node = nullptr;
            for (int i = 0; i < deleted_count; i++) {
                file.read(reinterpret_cast<char*>(&index), sizeofint);
                deleted_Node* temp = new deleted_Node(index, nullptr);
                if (last)
                    last->next = temp;
                else
                    deleted_node = temp;
                last = temp;
            }
            file.close();
            file.open(data_filename, std::ios::in | std::ios::out | std::ios::binary);
            if (!file) {
                file.open(data_filename, std::ios::out | std::ios::binary);
                file.close();
                file.open(data_filename, std::ios::in | std::ios::out | std::ios::binary);
            }
        }
    }

    ~BplusTree () {
        file.close();
        file.open(basic_filename, std::ios::out | std::ios::binary);
        file.seekp(0);
        file.write(reinterpret_cast<char*>(&Nodecount), sizeofint);
        file.write(reinterpret_cast<char*>(&root_index), sizeofint);
        file.write(reinterpret_cast<char*>(&deleted_count), sizeofint);
        deleted_Node *temp;
        for (int i = 0; i < deleted_count; i++) {
            file.write(reinterpret_cast<char*>(&(deleted_node->index)), sizeofint);
            temp = deleted_node;
            deleted_node = deleted_node->next;
            delete temp;
        }
        file.close();
    }

    void insert(const std::string &key, int value) {
        if (root_index == -1) {
            leaf_Node *root = new leaf_Node();
            root->count = 1;
            set_key(root->keys[0], key);
            root->values[0] = value;
            root->next_index = -1;
            root->index = EmptyNodeIndex();
            Nodecount = 1;
            root_index = root->index;
            writeNode(root);
            delete root;
            return;
        }

        auto [memory, found] = search(key, value);
        if (found) {
            delete memory;
            return;
        }
        
        leaf_Node *node = static_cast<leaf_Node*>(memory->node);
        int insert_pos = memory->index;
        int node_index = node->index;

        if (node->count > insert_pos) {
            std::memmove(node->keys[insert_pos + 1], node->keys[insert_pos], (node->count - insert_pos) * (strlength + 1));
            std::memmove(node->values + insert_pos + 1, node->values + insert_pos, (node->count - insert_pos) * sizeofint);
        }

        set_key(node->keys[insert_pos], key);
        node->values[insert_pos] = value;
        node->count++;
        if (node->count == Lsize) {
            leaf_Node *new_node = new leaf_Node();
            std::memcpy(new_node->keys, node->keys[half_Lsize], (Lsize - half_Lsize) * (strlength + 1));
            std::memcpy(new_node->values, node->values + half_Lsize, (Lsize - half_Lsize) * sizeofint);
            new_node->count = Lsize - half_Lsize;
            node->count = half_Lsize;
            new_node->index = EmptyNodeIndex();
            new_node->next_index = node->next_index;
            node->next_index = new_node->index;
            writeNode(node);
            writeNode(new_node);

            if (!memory->father) {
                internal_Node *new_root = new internal_Node();
                new_root->index = EmptyNodeIndex();
                set_key(new_root->keys[0], new_node->keys[0]);
                new_root->values[0] = new_node->values[0];
                new_root->nodes_index[0] = node_index;
                new_root->nodes_index[1] = new_node->index;
                new_root->count = 1;
                writeNode(new_root);
                root_index = new_root->index;
                delete new_root;
            } else {
                insert_internal(memory->father, new_node->keys[0], new_node->values[0], new_node->index);
            }
            delete new_node;
        } else {
            writeNode(node);
        }
        delete memory;
    }

    void remove(const std::string &key, int value) {
        if (root_index == -1)
            return;
        
        auto [memory, found] = search(key, value);
        if (!found) {
            delete memory;
            return;
        }

        leaf_Node *node = static_cast<leaf_Node*>(memory->node);
        int node_index = node->index;
        int erase_pos = memory->index;
        
        if (node->count - 1 > erase_pos) {
            std::memmove(node->keys[erase_pos], node->keys[erase_pos + 1], (node->count - 1 - erase_pos) * (strlength + 1));
            std::memmove(node->values + erase_pos, node->values + erase_pos + 1, (node->count - 1 - erase_pos) * sizeofint);
        }
        node->count--;
        writeNode(node);

        if (node->count < half_Lsize && node_index != root_index) {
            internal_Node *father = static_cast<internal_Node*>(memory->father->node);
            int index = memory->father->index;
            int right_index = -1, left_index = -1;
            leaf_Node *left = nullptr, *right = nullptr;
            if (index > 0) {
                left_index = father->nodes_index[index - 1];
                left = static_cast<leaf_Node*>(getNode(left_index));
                if (left->count > half_Lsize) {
                    // 向左借一个
                    std::memmove(node->keys[1], node->keys[0], node->count * (strlength + 1));
                    std::memmove(node->values + 1, node->values, node->count * sizeofint);
                    left->count--;
                    node->count++;
                    set_key(node->keys[0], left->keys[left->count]);
                    node->values[0] = left->values[left->count];

                    set_key(father->keys[index - 1], node->keys[0]);
                    father->values[index - 1] = node->values[0];

                    writeNode(node);
                    writeNode(left);
                    writeNode(father);
                    delete left;
                    delete right;
                    delete memory;
                    return;
                }
            }

            if (index < father->count) {
                right_index = father->nodes_index[index + 1];
                right = static_cast<leaf_Node*>(getNode(right_index));
                if (right->count > half_Lsize) {
                    // 向右借一个
                    set_key(node->keys[node->count], right->keys[0]);
                    node->values[node->count] = right->values[0];
                    std::memmove(right->keys[0], right->keys[1], (right->count - 1) * (strlength + 1));
                    std::memmove(right->values, right->values + 1, (right->count - 1) * sizeofint);
                    node->count++;
                    right->count--;

                    set_key(father->keys[index], right->keys[0]);
                    father->values[index] = right->values[0];

                    writeNode(node);
                    writeNode(right);
                    writeNode(father);
                    delete left;
                    delete right;
                    delete memory;
                    return;
                }
            }
            // 合并
            if (index > 0) { // 向左合并
                std::memcpy(left->keys + left->count, node->keys, node->count * (strlength + 1));
                std::memcpy(left->values + left->count, node->values, node->count * sizeofint);
                left->count += node->count;
                left->next_index = node->next_index;

                writeNode(left);
                add_deleted_Node(node_index);

                fatherRemove(father, index - 1, index);
                writeNode(father);

                if (father->index == root_index && father->count == 0) {
                    add_deleted_Node(root_index);
                    root_index = left_index;
                } else if(father->count < half_Isize && father->index != root_index) {
                    remove_internal(memory->father);
                }
            } else { // 向右合并
                std::memcpy(node->keys + node->count, right->keys, right->count * (strlength + 1));
                std::memcpy(node->values + node->count, right->values, right->count * sizeofint);
                node->count += right->count;
                node->next_index = right->next_index;

                writeNode(node);
                add_deleted_Node(right_index);

                fatherRemove(father, 0, 1);
                writeNode(father);

                if (father->index == root_index && father->count == 0) {
                    add_deleted_Node(root_index);
                    root_index = node_index;
                } else if (father->count < half_Isize && father->index != root_index) {
                    remove_internal(memory->father);
                }
            }
            delete left;
            delete right;
        } else if (node_index == root_index && node->count == 0) {
            root_index = -1;
            add_deleted_Node(node_index);
        }
        delete memory;
    }

    void find(const std::string &key) {
        if (root_index == -1) {
            std::cout << "null" << std::endl;
            return;
        }

        int index = root_index;
        while(true) {
            Node *temp = getNode(index);
            if (temp->type) {
                leaf_Node *node = static_cast<leaf_Node*>(temp);
                int l = 0, r = node->count, m;
                while (l != r) { // (l, r]
                    m = (l + r) >> 1;
                    if (keycmp(node->keys[m], key) >= 0)
                        r = m;
                    else
                        l = m + 1;
                }
                int pos = l;
                bool first = true;
                bool done = false;
                leaf_Node *cur = node;
                while (true) {
                    for (; pos < cur->count; ++pos) {
                        if (keycmp(cur->keys[pos], key) != 0) {
                            done = true;
                            break;
                        }
                        if (!first)
                            std::cout << ' ';
                        std::cout << cur->values[pos];
                        first = false;
                    }
                    if (done)
                        break;
                    if (cur->next_index == -1)
                        break;
                    int next_index = cur->next_index;
                    delete cur;
                    cur = static_cast<leaf_Node*>(getNode(next_index));
                    pos = 0;
                }
                if (first) std::cout << "null";
                std::cout << std::endl;
                delete cur;
                return;
            } else {
                internal_Node *node = static_cast<internal_Node*>(temp);
                int l = 0, r = node->count, m;
                while (l != r) {
                    m = (l + r) >> 1;
                    if (keycmp(node->keys[m], key) >= 0)
                        r = m;
                    else
                        l = m + 1;
                }
                int next_index = node->nodes_index[l];
                delete node;
                index = next_index;
            }
        }
    }

    void print(int index = -2) {
        if (index == -2) {
            index = root_index;
        }
        if (index == -1) {
            std::cout << "Empty!!!\n";
            return;
        }

        Node *temp = getNode(index);
        if (temp->type) {
            leaf_Node *node = static_cast<leaf_Node*>(temp);
            node->print();
        } else {
            internal_Node *node = static_cast<internal_Node*>(temp);
            node->print();
            for (int i = 0; i <= node->count; i++) {
                print(node->nodes_index[i]);
            }
        }
        delete temp;
    }
};