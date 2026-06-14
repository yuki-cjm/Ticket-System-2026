#pragma once

#include "utils/Error.hpp"

namespace sjtu {

template <class T, class Compare = bool (*)(const T&, const T&)>
class set {
  private:
    struct Node;
  public:
    class const_iterator;
    class iterator {
      private:
        set* s;
        Node* node;
        friend const_iterator;
        friend set;

      public:
        iterator() = default;

        iterator(const iterator &other) : s(other.s), node(other.node) {}

        iterator(set* s, Node* node) : s(s), node(node) {};

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
            if (!s) {
                throw invalid_iterator();
            }
            if (!node && s) {
                // the set is empty
                if (!s->root) {
                    throw invalid_iterator();
                }

                // the set is not empty
                node = s->root;
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
            if (!s) {
                throw invalid_iterator();
            }
            if (!node && s) {
                if (!s->root) {
                    throw invalid_iterator();
                }
                node = s->root;
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

        T &operator*() const {
            return node->data;
        }

        bool operator==(const iterator &rhs) const {
            return node == rhs.node && s == rhs.s;
        }

        bool operator==(const const_iterator &rhs) const {
            return node == rhs.node && s == rhs.s;
        }

        bool operator!=(const iterator &rhs) const {
            return node != rhs.node || s != rhs.s;
        }

        bool operator!=(const const_iterator &rhs) const {
            return node != rhs.node || s != rhs.s;
        }
    };
    class const_iterator {
      private:
        // data members.
        const set *s;
        const Node *node;
        friend iterator;
        
      public:
        const_iterator() = default;

        const_iterator(const const_iterator &other) {
            s = other.s;
            node = other.node;
        }

        const_iterator(const iterator &other) {
            s = other.s;
            node = other.node;
        }

        const_iterator(const set *s, const Node *node) : s(s), node(node) {}

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
            if (!s) {
                throw invalid_iterator();
            }
            if (!node && s) {
                // the set is empty
                if (!s->root) {
                    throw invalid_iterator();
                }
                // the set is not empty
                node = s->root;
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
            if (!s) {
                throw invalid_iterator();
            }
            if (!node && s) {
                if (!s->root) {
                    throw invalid_iterator();
                }
                node = s->root;
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

        const T &operator*() const {
            return node->data;
        }

        bool operator==(const iterator &rhs) const {
            return node == rhs.node && s == rhs.s;
        }

        bool operator==(const const_iterator &rhs) const {
            return node == rhs.node && s == rhs.s;
        }

        bool operator!=(const iterator &rhs) const {
            return node != rhs.node || s != rhs.s;
        }

        bool operator!=(const const_iterator &rhs) const {
            return node != rhs.node || s != rhs.s;
        }
    };

  private:
    struct Node {
        T data;
        Node *left, *right, *father;
        bool color; // true == Black, false == Red
        Node(const T &data) : data(data), left(nullptr), right(nullptr), father(nullptr), color(false) {}
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

    void removeAdjust(Node *&node, const T *&data) {
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
            if (!compare(node->data, *data) && !compare(*data, node->data)) {

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
                if (compare(node->data, *data)) {
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
                    removeAdjust(node, data);
                }
            }
        }
    }

  public:
    /**
     * TODO two constructors
     */
    set(Compare cmp) : compare(cmp) {
        root = nullptr;
        begin_ = nullptr;
        size_ = 0;
    }

    set(const set &other) : compare(other.compare) {
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
    set &operator=(const set &other) {
        if (this == &other) {
            return *this;
        }
        compare = other.compare;
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
    ~set() {
        if (root) {
            makeEmpty(root);
        }
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
    pair<iterator, bool> insert(const T &data) {
        if (!root) {
            root = new Node(data);
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
                if (compare(node->data, data)) {
                    father = node;
                    node = node->right;
                }
                else if (compare(data, node->data)) {
                    father = node;
                    node = node->left;
                }
                else {
                    return pair(iterator(this, node), false);
                }
            }
            else {
                node = new Node(data);
                node->father = father;

                node->color = false;
                node->left = node->right = nullptr;
                if (compare(data, father->data)) {
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
        if (!pos.node || pos.s != this) {
            throw invalid_iterator();
        }
        const T *data = &(pos.node->data);
        if (!root) {
            throw invalid_iterator();
        }
        if (!compare(root->data, *data) && !compare(*data, root->data) && !root->left && !root->right) {
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
            removeAdjust(node, data);
            if (!compare(node->data, *data) && !compare(*data, node->data)) {
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
                    node->data.~T();
                    new (&node->data) T(tmp->data);
                    data = &(node->data);
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
            if (compare(node->data, *data)) {
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
    size_t count(const T &data) const {
        if (!root) {
            return 0;
        }
        Node *node = root;
        while (true) {
            if (compare(node->data, data)) {
                if (node->right) {
                    node = node->right;
                }
                else {
                    return 0;
                }
            }
            else if (compare(data, node->data)) {
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
    iterator find(const T &data) {
        if (!root) {
            return iterator(this, nullptr);
        }
        Node *node = root;
        while (true) {
            if (compare(node->data, data)) {
                if (node->right) {
                    node = node->right;
                }
                else {
                    return iterator(this, nullptr);
                }
            }
            else if (compare(data, node->data)) {
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

    const_iterator find(const T &data) const {
        if (!root) {
            return const_iterator(this, nullptr);
        }
        Node *node = root;
        while (true) {
            if (compare(node->data, data)) {
                if (node->right) {
                    node = node->right;
                }
                else {
                    return const_iterator(this, nullptr);
                }
            }
            else if (compare(data, node->data)) {
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

} // namespace sjtu