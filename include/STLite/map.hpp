#pragma once

// only for std::less<T>
#include <functional>

#include "utils/Error.hpp"
#include "STLite/pair.hpp"

namespace sjtu {

//this map's insert and erase functions adjust the structrue of RedBlackTree from the top to the bottom
template <class Key, class T, class Compare = std::less <Key>>
class map {
  private:
    struct Node;
  public:
    /**
     * the internal type of data.
     * it should have a default constructor, a copy constructor.
     * You can use sjtu::map as value_type by typedef.
     */
    typedef pair<const Key, T> value_type;
    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = map.begin(); --it;
     *       or it = map.end(); ++end();
     */
    class const_iterator;
    class iterator {
      private:
        map* m;
        Node* node;
        friend const_iterator;
        friend map;

      public:
        iterator() = default;

        iterator(const iterator &other) : m(other.m), node(other.node) {}

        iterator(map* m, Node* node) : m(m), node(node) {};

        iterator operator++(int) {
            if (node == nullptr) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            if (node->right) {
                node = node->right;
                while (node->left) {
                    node = node->left;
                }
                return tmp;
            }
            while (node->father) {
                if (node->father->left == node) {
                    node = node->father;
                    return tmp;
                }
                node = node->father;
            }
            node = nullptr;
            return tmp;
        }

        iterator &operator++() {
            if (node == nullptr) {
                throw invalid_iterator();
            }
            if (node->right) {
                node = node->right;
                while (node->left) {
                    node = node->left;
                }
                return *this;
            }
            while (node->father) {
                if (node->father->left == node) {
                    node = node->father;
                    return *this;
                }
                node = node->father;
            }
            node = nullptr;
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            if (!m) {
                throw invalid_iterator();
            }
            if (!node && m) {
                // the map is empty
                if (!m->root) {
                    throw invalid_iterator();
                }
                // the map is not empty
                node = m->root;
                while (node->right) {
                    node = node->right;
                }
                return tmp;
            }
            if (node->left) {
                node = node->left;
                while (node->right) {
                    node = node->right;
                }
                return tmp;
            }
            while (node->father) {
                if (node->father->right == node) {
                    node = node->father;
                    return tmp;
                }
                node = node->father;
            }
            throw invalid_iterator();
        }

        iterator &operator--() {
            if (!m) {
                throw invalid_iterator();
            }
            if (!node && m) {
                if (!m->root) {
                    throw invalid_iterator();
                }
                node = m->root;
                while (node->right) {
                    node = node->right;
                }
                return *this;
            }
            if (node->left) {
                node = node->left;
                while (node->right) {
                    node = node->right;
                }
                return *this;
            }
            while (node->father) {
                if (node->father->right == node) {
                    node = node->father;
                    return *this;
                }
                node = node->father;
            }
            throw invalid_iterator();
        }

        value_type &operator*() const {
            return node->data;
        }

        bool operator==(const iterator &rhs) const {
            return node == rhs.node && m == rhs.m;
        }

        bool operator==(const const_iterator &rhs) const {
            return node == rhs.node && m == rhs.m;
        }

        bool operator!=(const iterator &rhs) const {
            return node != rhs.node || m != rhs.m;
        }

        bool operator!=(const const_iterator &rhs) const {
            return node != rhs.node || m != rhs.m;
        }

        /**
         * for the support of it->first.
         * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
         */
        value_type *operator->() const noexcept {
            return &node->data;
        }
    };
    class const_iterator {
      private:
        // data members.
        const map *m;
        const Node *node;
        friend iterator;
        
      public:
        const_iterator() = default;

        const_iterator(const const_iterator &other) {
            m = other.m;
            node = other.node;
        }

        const_iterator(const iterator &other) {
            m = other.m;
            node = other.node;
        }

        const_iterator(const map *m, const Node *node) : m(m), node(node) {}

        const_iterator operator++(int) {
            if (node == nullptr) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            if (node->right) {
                node = node->right;
                while (node->left) {
                    node = node->left;
                }
                return tmp;
            }
            while (node->father) {
                if (node->father->left == node) {
                    node = node->father;
                    return tmp;
                }
                node = node->father;
            }
            node = nullptr;
            return tmp;
        }

        const_iterator &operator++() {
            if (node == nullptr) {
                throw invalid_iterator();
            }
            if (node->right) {
                node = node->right;
                while (node->left) {
                    node = node->left;
                }
                return *this;
            }
            while (node->father) {
                if (node->father->left == node) {
                    node = node->father;
                    return *this;
                }
                node = node->father;
            }
            node = nullptr;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            if (!m) {
                throw invalid_iterator();
            }
            if (!node && m) {
                // the map is empty
                if (!m->root) {
                    throw invalid_iterator();
                }
                // the map is not empty
                node = m->root;
                while (node->right) {
                    node = node->right;
                }
                return tmp;
            }
            if (node->left) {
                node = node->left;
                while (node->right) {
                    node = node->right;
                }
                return tmp;
            }
            while (node->father) {
                if (node->father->right == node) {
                    node = node->father;
                    return tmp;
                }
                node = node->father;
            }
            throw invalid_iterator();
        }

        const_iterator &operator--() {
            if (!m) {
                throw invalid_iterator();
            }
            if (!node && m) {
                if (!m->root) {
                    throw invalid_iterator();
                }
                node = m->root;
                while (node->right) {
                    node = node->right;
                }
                return *this;
            }
            if (node->left) {
                node = node->left;
                while (node->right) {
                    node = node->right;
                }
                return *this;
            }
            while (node->father) {
                if (node->father->right == node) {
                    node = node->father;
                    return *this;
                }
                node = node->father;
            }
            throw invalid_iterator();
        }

        const value_type &operator*() const {
            return node->data;
        }

        bool operator==(const iterator &rhs) const {
            return node == rhs.node && m == rhs.m;
        }

        bool operator==(const const_iterator &rhs) const {
            return node == rhs.node && m == rhs.m;
        }

        bool operator!=(const iterator &rhs) const {
            return node != rhs.node || m != rhs.m;
        }

        bool operator!=(const const_iterator &rhs) const {
            return node != rhs.node || m != rhs.m;
        }

        /**
         * for the support of it->first.
         * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
         */
        const value_type *operator->() const noexcept {
            return &node->data;
        }
    };

  private:
    struct Node {
        value_type data;
        Node *left, *right, *father;
        bool color; // true == Black, false == Red
        Node(const value_type &data) : data(data), left(nullptr), right(nullptr), father(nullptr), color(false) {}
    };
    Node *root, *begin_;
    size_t size_;
    Compare compare;

    Node *copy(Node *other) {
        Node *node = new Node(other->data);
        if (other->left) {
            node->left = copy(other->left);
            node->left->father = node;
        }
        else {
            node->left = nullptr;
        }
        if (other->right) {
            node->right = copy(other->right);
            node->right->father = node;
        }
        else {
            node->right = nullptr;
        }
        node->color = other->color;
        return node;
    }

    void makeEmpty(Node *node) {
        if (node->left) {
            makeEmpty(node->left);
        }
        if (node->right) {
            makeEmpty(node->right);
        }
        delete node;
    }

    T &tryFind(const Key &key) const {
        if (!root) {
            throw index_out_of_bound();
        }
        Node *node = root;
        while (true) {
            if (compare(node->data.first, key)) {
                if (node->right) {
                    node = node->right;
                    continue;
                }
                else {
                    throw index_out_of_bound();
                }
            }
            if (compare(key, node->data.first)) {
                if (node->left) {
                    node = node->left;
                    continue;
                }
                else {
                    throw index_out_of_bound();
                }
            }
            return node->data.second;
        }
    }

    /*
            1         2
           / \       / \
          2   3 --> 4   1
         / \           / \
        4   5         5   3
    */
    void LL(Node *node) {
        Node *tmp = node->left;
        if (root == node) {
            root = tmp;
        }
        tmp->father = node->father;
        if (node->father) {
            if (node->father->left == node) {
                node->father->left = tmp;
            }
            else {
                node->father->right = tmp;
            }
        }
        if (tmp->right) {
            tmp->right->father = node;
        }
        node->left = tmp->right;
        node->father = tmp;
        tmp->right = node;
        std::swap(tmp->color, node->color);
    }

    void RR(Node *node) {
        Node *tmp = node->right;
        if (root == node) {
            root = tmp;
        }
        tmp->father = node->father;
        if (node->father) {
            if (node->father->right == node) {
                node->father->right = tmp;
            }
            else {
                node->father->left = tmp;
            }
        }
        if (tmp->left) {
            tmp->left->father = node;
        }
        node->right = tmp->left;
        node->father = tmp;
        tmp->left = node;
        std::swap(tmp->color, node->color);
    }

    void LR(Node *node) {
        RR(node->left);
        LL(node);
    }

    void RL(Node *node) {
        LL(node->right);
        RR(node);
    }

    void insertAdjust(Node *node, Node *&son) {
        if (node->color) return;
        if (node == root) {
            node->color = true;
            return;
        }
        if (node->father->left == node) {
            if (node->left == son) {
                LL(node->father);
            }
            else {
                LR(node->father);
            }
        }
        else {
            if (node->left == son) {
                RL(node->father);
            }
            else {
                RR(node->father);
            }
        }
    }

    void removeAdjust(Node *&node, const Key *&key) {
        if (!node->color) {
            return;
        }
        if (node == root) {
            if (node->left && node->right && node->left->color == node->right->color) {
                node->color = false;
                node->left->color = node->right->color = true;
                return;
            }
        }
        if ((node->left && node->left->color || !node->left) && (node->right && node->right->color || !node->right)) {
            Node *brother;
            if (node->father->left == node) {
                brother = node->father->right;
            }
            else {
                brother = node->father->left;
            }
            if ((brother->left && brother->left->color || !brother->left) && (brother->right && brother->right->color || !brother->right)) {
                node->father->color = true;
                node->color = brother->color = false;
            }
            else {
                if (node->father->left == brother) {
                    /*
                             1R                     1R                   2R                   2R
                            /  \        c4         /  \       LL        /  \        c3       /  \
                          2B    3B*   ----->     2B    3B*  ----->     4B   1B    ----->   4B   1B
                         / \    / \             / \    / \                 / \                  / \
                        4R  ?  5B 6B           4B  ?  5B 6B               ?   3B*              ?   3R*
                    */
                    if (brother->left && !brother->left->color) {
                        brother->left->color = true;
                        LL(node->father);
                    }
                    /*
                             1R                   1R                  1R                   4R                  4R
                            /  \        c4       /  \       LR1      /  \        LR2      /  \        c3      /  \
                          2B    3B*   ----->   2B    3B*  ------>   4B   3B*   ------>   2B   1B    ----->   2B   1B
                         / \    / \           / \    / \           /     / \                   \                   \
                        ?  4R  5B 6B         ?  4B  5B 6B         2B    5B  6B                  3B*                 3R*
                    */
                    else {
                        brother->right->color = true;
                        LR(node->father);
                    }
                }
                else {
                    if (brother->right && !brother->right->color) {
                        brother->right->color = true;
                        RR(node->father);
                    }
                    else {
                        brother->left->color = true;
                        RL(node->father);
                    }
                }
                node->color = false;
            }
        }
        else {
            if (!compare(node->data.first, *key) && !compare(*key, node->data.first)) {
                if (node->left && node->right) {
                    /*
                           1B*      LL     2B
                          /  \    ----->     \
                         2R  3B               1R*
                                               \
                                                3B
                    */
                    if (node->right->color) {
                        LL(node);
                    }
                    return ;
                }
                /*
                        1B*     LL      2B
                       / \    ----->      \
                      2R  -               1R*
                */
                if (node->left) {
                    LL(node);
                }
                else {
                    RR(node);
                }
            }
            else {
                /*
                         1B*      cn       1B       RR        3B
                        /  \    ----->    /  \    ----->     /      -----> adjust again
                       2B  3R            2B* 3R             1R
                                                           /
                                                          2B*
                */
                if (compare(node->data.first, *key)) {
                    node = node->right;
                }
                else {
                    node = node->left;
                }
                if (!node) {
                    throw invalid_iterator();
                }
                if (node->color) {
                    if (node == node->father->left) {
                        RR(node->father);
                    }
                    else {
                        LL(node->father);
                    }
                    removeAdjust(node, key);
                }
            }
        }
    }

  public:
    /**
     * TODO two constructors
     */
    map() {
        root = nullptr;
        begin_ = nullptr;
        size_ = 0;
    }

    map(const map &other) {
        if (other.root) {
            root = copy(other.root);
            root->father = nullptr;
            size_ = other.size_;
            begin_ = root;
            while (begin_->left) {
                begin_ = begin_->left;
            }
        }
        else {
            root = nullptr;
            begin_ = nullptr;
            size_ = 0;
        }
    }

    /**
     * TODO assignment operator
     */
    map &operator=(const map &other) {
        if (this == &other) {
            return *this;
        }
        if (root) {
            makeEmpty(root);
        }
        if (other.root) {
            root = copy(other.root);
            begin_ = root;
            while(begin_->left) {
                begin_ = begin_->left;
            }
            root->father = nullptr;
        }
        else {
            root = nullptr;
            begin_ = nullptr;
        }
        size_ = other.size_;
        return *this;
    }

    /**
     * TODO Destructors
     */
    ~map() {
        if (root) {
            makeEmpty(root);
        }
    }

    /**
     * TODO
     * access specified element with bounds checking
     * Returns a reference to the mapped value of the element with key equivalent to key.
     * If no such element exists, an exception of type `index_out_of_bound'
     */
    T &at(const Key &key) {
        return tryFind(key);
    }

    const T &at(const Key &key) const {
        return tryFind(key);
    }

    /**
     * TODO
     * access specified element
     * Returns a reference to the value that is mapped to a key equivalent to key,
     *   performing an insertion if such key does not already exist.
     */
    T &operator[](const Key &key) {
        iterator it = find(key);
        if (it == end()) {
            it = insert(value_type(key, T())).first;
        }
        return it->second;
    }

    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T &operator[](const Key &key) const {
        return tryFind(key);
    }

    /**
     * return a iterator to the beginning
     */
    iterator begin() {
        return iterator(this, begin_);
    }

    const_iterator cbegin() const {
        return const_iterator(this, begin_);
    }

    iterator end() {
        return iterator(this, nullptr);
    }

    const_iterator cend() const {
        return const_iterator(this, nullptr);
    }

    bool empty() const {
        return size_ == 0;
    }

    size_t size() const {
        return size_;
    }

    /**
     * clears the contents
     */
    void clear() {
        if (root) {
            makeEmpty(root);
        }
        root = nullptr;
        begin_ = nullptr;
        size_ = 0;
    }

    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the insertion),
     *   the second one is true if insert successfully, or false.
     */
    pair<iterator, bool> insert(const value_type &value) {
        if (!root) {
            root = new Node(value);
            root->color = true;
            root->left = root->right = root->father = nullptr;
            begin_ = root;
            size_++;
            return pair(iterator(this, root), true);
        }
        Node *node = root, *father = nullptr;
        while (true) {
            if (node) {
                if (node->left && !node->left->color && node->right && !node->right->color) {
                    node->left->color = node->right->color = true;
                    if (node == root) {
                        node->color = true;
                    }
                    else {
                        node->color = false;
                        insertAdjust(node->father, node);
                    }
                }
                if (compare(node->data.first, value.first)) {
                    father = node;
                    node = node->right;
                }
                else if (compare(value.first, node->data.first)) {
                    father = node;
                    node = node->left;
                }
                else {
                    return pair(iterator(this, node), false);
                }
            }
            else {
                node = new Node(value);
                node->father = father;
                node->color = false;
                node->left = node->right = nullptr;
                if (compare(value.first, father->data.first)) {
                    father->left = node;
                    if (father == begin_) {
                        begin_ = node;
                    }
                }
                else {
                    father->right = node;
                }
                insertAdjust(father, node);
                root->color = true;
                size_++;
                return pair(iterator(this, node), true);
            }
        }
    }

    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase(iterator pos) {
        if (!pos.node || pos.m != this) {
            throw invalid_iterator();
        }
        const Key *key = &(pos.node->data.first);
        if (!root) {
            throw invalid_iterator();
        }
        if (!compare(root->data.first, *key) && !compare(*key, root->data.first) && !root->left && !root->right) {
            if (root != pos.node) {
                throw invalid_iterator();
            }
            else {
                delete root;
                root = nullptr;
                begin_ = nullptr;
                size_--;
                return;
            }
        }

        Node* node = root;
        while (true) {
            if (!node) {
                throw invalid_iterator();
            }
            removeAdjust(node, key);
            if (!compare(node->data.first, *key) && !compare(*key, node->data.first)) {
                if (node->left && node->right) {
                    Node *tmp = node->right;
                    while (tmp->left) {
                        tmp = tmp->left;
                    }
                    if (root == node) {
                        root = tmp;
                    }
                    if (node->father) {
                        if (node->father->left == node) {
                            node->father->left = tmp;
                        }
                        else {
                            node->father->right = tmp;
                        }
                    }
                    node->left->father = tmp;
                    if (tmp->right) {
                        tmp->right->father = node;
                    }
                    std::swap(node->color, tmp->color);
                    if (tmp != node->right) {
                        node->right->father = tmp;
                        tmp->father->left = node;
                        std::swap(node->father, tmp->father);
                        std::swap(node->left, tmp->left);
                        std::swap(node->right, tmp->right);
                    }
                    else {
                        tmp->father = node->father;
                        node->father = tmp;
                        std::swap(node->left, tmp->left);
                        node->right = tmp->right;
                        tmp->right = node;
                    }
                    node->data.~value_type();
                    new (&node->data) value_type(tmp->data);
                    key = &(node->data.first);
                    node = tmp->right;
                    continue;
                }
                else {
                    if (node == begin_) {
                        begin_ = node->father;
                    }
                    if (node->father) {
                        if (node->father->left == node) {
                            if (node->left) {
                                node->father->left = node->left;
                                node->left->father = node->father;
                            }
                            else if (node->right) {
                                node->father->left = node->right;
                                node->right->father = node->father;
                            }
                            else {
                                node->father->left = nullptr;
                            }
                        }
                        else {
                            if (node->left) {
                                node->father->right = node->left;
                                node->left->father = node->father;
                            }
                            else if (node->right) {
                                node->father->right = node->right;
                                node->right->father = node->father;
                            }
                            else {
                                node->father->right = nullptr;
                            }
                        }
                    }
                    else {
                        if (node->left) {
                            node->left->father = nullptr;
                            root = node->left;
                        }
                        else {
                            node->right->father = nullptr;
                            root = node->right;
                        }
                    }
                    delete node;
                    node = nullptr;
                    root->color = true;
                    size_--;
                    return;
                }
            }
            if (compare(node->data.first, *key)) {
                node = node->right;
            }
            else {
                node = node->left;
            }
        }
    }

    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key &key) const {
        if (!root) {
            return 0;
        }
        Node *node = root;
        while (true) {
            if (compare(node->data.first, key)) {
                if (node->right) {
                    node = node->right;
                }
                else {
                    return 0;
                }
            }
            else if (compare(key, node->data.first)) {
                if (node->left) {
                    node = node->left;
                }
                else {
                    return 0;
                }
            }
            else {
                return 1;
            }
        }
    }

    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is returned.
     */
    iterator find(const Key &key) {
        if (!root) {
            return iterator(this, nullptr);
        }
        Node *node = root;
        while (true) {
            if (compare(node->data.first, key)) {
                if (node->right) {
                    node = node->right;
                }
                else {
                    return iterator(this, nullptr);
                }
            }
            else if (compare(key, node->data.first)) {
                if (node->left) {
                    node = node->left;
                }
                else {
                    return iterator(this, nullptr);
                }
            }
            else {
                return iterator(this, node);
            }
        }
    }

    const_iterator find(const Key &key) const {
        if (!root) {
            return const_iterator(this, nullptr);
        }
        Node *node = root;
        while (true) {
            if (compare(node->data.first, key)) {
                if (node->right) {
                    node = node->right;
                }
                else {
                    return const_iterator(this, nullptr);
                }
            }
            else if (compare(key, node->data.first)) {
                if (node->left) {
                    node = node->left;
                }
                else {
                    return const_iterator(this, nullptr);
                }
            }
            else {
                return const_iterator(this, node);
            }
        }
    }
};

}