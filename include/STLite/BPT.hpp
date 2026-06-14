#pragma once

#include <fstream>
#include <cstring>

#include "vector.hpp"
#include "string.hpp"

template<class T, class U>
class BplusTree {
  private:
    static constexpr int sizeofint = sizeof(int);
    static constexpr int sizeofT = sizeof(T);
    static constexpr int sizeofU = sizeof(U);

    static constexpr int desired_fanout = 64;
    static constexpr int raw_node = (1 + sizeofint * 3) + desired_fanout * (sizeofT + sizeofU + sizeofint) + sizeofint;
    static constexpr int sizeofNode = raw_node <= 4096 ? 4096 : ((raw_node + 4095) / 4096) * 4096;
    static constexpr int cache_size = 1024;

    static constexpr int Isize = (sizeofNode - 1 - sizeofint * 3) / (sizeofT + sizeofU + sizeofint);
    static constexpr int Lsize = (sizeofNode - 1 - sizeofint * 3) / (sizeofT + sizeofU);

    static constexpr int half_Isize = Isize >> 1;
    static constexpr int half_Lsize = Lsize >> 1;
  private:
    class Node {
      public:
        bool type;
        int index, count;

        virtual void read(const char *buf) = 0;
        virtual void serialize(char *buf) = 0;
        virtual ~Node() = default;
    };

    class internal_Node : public Node {
      public:
        T keys[Isize + 1];
        U values[Isize + 1];
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
                std::memcpy(keys, p, this->count * sizeofT);
                p += this->count * sizeofT;
                std::memcpy(values, p, this->count * sizeofU);
                p += this->count * sizeofU;
            }
            std::memcpy(nodes_index, p, (this->count + 1) * sizeofint);
        }

        void serialize(char *buf) override {
            char *p = buf;
            std::memcpy(p, &this->type, 1);   p += 1;
            std::memcpy(p, &this->count, sizeofint);  p += sizeofint;
            std::memcpy(p, &this->index, sizeofint);  p += sizeofint;
            if (this->count > 0) {
                std::memcpy(p, keys, this->count * sizeofT);
                p += this->count * sizeofT;
                std::memcpy(p, values, this->count * sizeofU);
                p += this->count * sizeofU;
            }
            std::memcpy(p, nodes_index, (this->count + 1) * sizeofint);
        }
    };

    class leaf_Node : public Node {
      public:
        int next_index;
        T keys[Lsize];
        U values[Lsize];

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
            if (this->count > 0) {
                std::memcpy(keys, p, this->count * sizeofT);
                p += this->count * sizeofT;
                std::memcpy(values, p, this->count * sizeofU);
                p += this->count * sizeofU;
            }
        }

        void serialize(char *buf) override {
            char *p = buf;
            std::memcpy(p, &this->type, 1);p += 1;
            std::memcpy(p, &this->count, sizeofint); p += sizeofint;
            std::memcpy(p, &this->index, sizeofint); p += sizeofint;
            std::memcpy(p, &next_index, sizeofint); p += sizeofint;
            if (this->count > 0) {
                std::memcpy(p, keys, this->count * sizeofT);
                p += this->count * sizeofT;
                std::memcpy(p, values, this->count * sizeofU);
                p += this->count * sizeofU;
            }
        }
    };


    static int keycmp(const T &a, const T &b) {
        if (a < b) return -1;
        if (a == b) return 0;
        return 1;
    }

    static void set_key(T &dest, const T &src) {
        dest = src;
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
    };
    Cache *cache;  // 缓存数组在堆上分配，规模为 cache_size


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


    std::pair<Memory*, bool> search(const T &key, const U &value) { // (a, b) -> b
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
                    if (cmp > 0 || (cmp == 0 && node->values[m] > value))
                        r = m;
                    else if (cmp < 0 || (cmp == 0 && node->values[m] < value))
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
                    if (cmp > 0 || (cmp == 0 && node->values[m] > value))
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
    void insert_internal(Memory *memory, const T &key, const U &value, int child_index) {
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

        std::memmove(node->keys + pos + 1, node->keys + pos, (node->count - pos) * sizeofT);
        std::memmove(node->values + pos + 1, node->values + pos, (node->count - pos) * sizeofU);
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
            std::memcpy(new_node->keys, node->keys + half_Isize + 1, new_node->count * sizeofT);
            std::memcpy(new_node->values, node->values + half_Isize + 1, new_node->count * sizeofU);
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
        std::memmove(node->keys + data_index, node->keys + data_index + 1, (node->count - 1 - data_index) * sizeofT);
        std::memmove(node->values + data_index, node->values + data_index + 1, (node->count - 1 - data_index) * sizeofU);
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
                std::memmove(node->keys + 1, node->keys, node->count * sizeofT);
                std::memmove(node->values + 1, node->values, node->count * sizeofU);
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
                std::memmove(right->keys, right->keys + 1, (right->count - 1) * sizeofT);
                std::memmove(right->values, right->values + 1, (right->count - 1) * sizeofU);
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
            std::memcpy(left->keys + left->count, node->keys, node->count * sizeofT);
            std::memcpy(left->values + left->count, node->values, node->count * sizeofU);
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
            std::memcpy(node->keys + node->count, right->keys, right->count * sizeofT);
            std::memcpy(node->values + node->count, right->values, right->count * sizeofU);
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
        cache = new Cache[cache_size];
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
        delete[] cache;
    }


    void insert(const T &key, const U &value) {
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
            std::memmove(node->keys + insert_pos + 1, node->keys + insert_pos, (node->count - insert_pos) * sizeofT);
            std::memmove(node->values + insert_pos + 1, node->values + insert_pos, (node->count - insert_pos) * sizeofU);
        }

        set_key(node->keys[insert_pos], key);
        node->values[insert_pos] = value;
        node->count++;
        if (node->count == Lsize) {
            leaf_Node *new_node = new leaf_Node();
            std::memcpy(new_node->keys, node->keys + half_Lsize, (Lsize - half_Lsize) * sizeofT);
            std::memcpy(new_node->values, node->values + half_Lsize, (Lsize - half_Lsize) * sizeofU);
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

    void remove(const T &key, const U &value) {
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
            std::memmove(node->keys + erase_pos, node->keys + erase_pos + 1, (node->count - 1 - erase_pos) * sizeofT);
            std::memmove(node->values + erase_pos, node->values + erase_pos + 1, (node->count - 1 - erase_pos) * sizeofU);
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
                    std::memmove(node->keys + 1, node->keys, node->count * sizeofT);
                    std::memmove(node->values + 1, node->values, node->count * sizeofU);
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
                    std::memmove(right->keys, right->keys + 1, (right->count - 1) * sizeofT);
                    std::memmove(right->values, right->values + 1, (right->count - 1) * sizeofU);
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
                std::memcpy(left->keys + left->count, node->keys, node->count * sizeofT);
                std::memcpy(left->values + left->count, node->values, node->count * sizeofU);
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
                std::memcpy(node->keys + node->count, right->keys, right->count * sizeofT);
                std::memcpy(node->values + node->count, right->values, right->count * sizeofU);
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

    sjtu::vector<U> find(const T &key) {
        sjtu::vector<U> ans;
        if (root_index == -1) {
            return ans;
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
                bool done = false;
                leaf_Node *cur = node;
                while (true) {
                    for (; pos < cur->count; ++pos) {
                        if (keycmp(cur->keys[pos], key) != 0) {
                            done = true;
                            break;
                        }
                        ans.push_back(cur->values[pos]);
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
                delete cur;
                return ans;
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

    void clear() {
        file.close();
        file.open(data_filename, std::ios::out | std::ios::trunc | std::ios::binary);
        file.close();
        file.open(basic_filename, std::ios::out | std::ios::trunc | std::ios::binary);
        file.close();
        deleted_Node *temp;
        for (int i = 0; i < deleted_count; i++) {
            temp = deleted_node;
            deleted_node = deleted_node->next;
            delete temp;
        }
        for (int i = 0; i < cache_size; i++) {
            cache[i].node_index = -1;
        }
        file.open(data_filename, std::ios::in | std::ios::out | std::ios::binary);
        Nodecount = 0;
        root_index = -1;
        deleted_count = 0;
        deleted_node = nullptr;
    }
};
