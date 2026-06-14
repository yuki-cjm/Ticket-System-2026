#pragma once

#include "utils/Error.hpp"

namespace sjtu {

const int chunk_size = 16;
const int initial_size = 4;

template <class T> class deque {
public:
  class const_iterator;
  class iterator {
  private:
    /**
     * TODO add data members
     *   just add whatever you want.
     */
    deque<T> *deq_;
    int location_;
    friend class const_iterator;
    friend class deque<T>;

  public:
    /**
     * return a new iterator which pointer n-next elements
     *   even if there are not enough elements, the behaviour is **undefined**.
     * as well as operator-
     */
    iterator() = default;
    iterator(deque<T> *deq, int location) : deq_(deq), location_(location) {}
    iterator operator+(const int &n) const {
      // TODO
      iterator temp;
      temp.deq_ = deq_;
      temp.location_ = location_ + n;
      return temp;
    }
    iterator operator-(const int &n) const {
      // TODO
      iterator temp;
      temp.deq_ = deq_;
      temp.location_ = location_ - n;
      return temp;
    }
    // return th distance between two iterator,
    // if these two iterators points to different vectors, throw
    // invaild_iterator.
    int operator-(const iterator &rhs) const {
      // TODO
      if (deq_ != rhs.deq_)
        throw invalid_iterator();
      return location_ - rhs.location_;
    }
    iterator operator+=(const int &n) {
      // TODO
      location_ += n;
      return *this;
    }
    iterator operator-=(const int &n) {
      // TODO
      location_ -= n;
      return *this;
    }
    /**
     * TODO iter++
     */
    iterator operator++(int) {
      iterator temp = *this;
      location_++;
      return temp;
    }
    /**
     * TODO ++iter
     */
    iterator &operator++() {
      location_++;
      return *this;
    }
    /**
     * TODO iter--
     */
    iterator operator--(int) {
      iterator temp = *this;
      location_--;
      return temp;
    }
    /**
     * TODO --iter
     */
    iterator &operator--() {
      location_--;
      return *this;
    }
    /**
     * TODO *it
     */
    T &operator*() const { return (*deq_)[location_]; }
    /**
     * TODO it->field
     */
    T *operator->() const noexcept { return &(*deq_)[location_]; }
    /**
     * a operator to check whether two iterators are same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const {
      return deq_ == rhs.deq_ && location_ == rhs.location_;
    }
    bool operator==(const const_iterator &rhs) const {
      return deq_ == rhs.deq_ && location_ == rhs.location_;
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return deq_ != rhs.deq_ || location_ != rhs.location_;
    }
    bool operator!=(const const_iterator &rhs) const {
      return deq_ != rhs.deq_ || location_ != rhs.location_;
    }
  };
  class const_iterator {
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
  private:
    // data members.
    const deque<T> *deq_;
    int location_;

  public:
    const_iterator() = default;
    const_iterator(deque<T> *deq, int location)
        : deq_(deq), location_(location) {}
    const_iterator(const deque<T> *deq, int location)
        : deq_(deq), location_(location) {}
    const_iterator(const iterator &other) {
      // TODO
      deq_ = other.deq_;
      location_ = other.location_;
    }
    const_iterator operator+(const int &n) const {
      // TODO
      return const_iterator(deq_, location_ + n);
    }
    const_iterator operator-(const int &n) const {
      // TODO
      return const_iterator(deq_, location_ - n);
    }
    int operator-(const const_iterator &rhs) const {
      // TODO
      return location_ - rhs.location_;
    }
    const_iterator operator+=(const int &n) {
      // TODO
      location_ += n;
      return *this;
    }
    const_iterator operator-=(const int &n) {
      // TODO
      location_ -= n;
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator temp = *this;
      location_++;
      return temp;
    }
    const_iterator &operator++() {
      location_++;
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator temp = *this;
      location_--;
      return temp;
    }
    const_iterator &operator--() {
      location_--;
      return *this;
    }
    const T &operator*() const { return (*deq_)[location_]; }
    const T *operator->() const noexcept { return &(*deq_)[location_]; }
    bool operator==(const iterator &rhs) const {
      return deq_ == rhs.deq_ && location_ == rhs.location_;
    }
    bool operator==(const const_iterator &rhs) const {
      return deq_ == rhs.deq_ && location_ == rhs.location_;
    }
    bool operator!=(const iterator &rhs) const {
      return deq_ != rhs.deq_ || location_ == rhs.location_;
    }
    bool operator!=(const const_iterator &rhs) const {
      return deq_ != rhs.deq_ || location_ != rhs.location_;
    }
  };

private:
  friend class iterator;
  friend class const_iterator;

  struct Chunk {
    T *data;
  };
  Chunk *chunk_;
  int begin_, end_; // [begin_ , end_)
  int size_, capacity_;
  bool full() { return size_ == capacity_ * chunk_size - 1; }
  void doubleCapacity() {
    Chunk *temp_chunk = new Chunk[2 * capacity_];
    if (end_ >= begin_) // begin_ == 0 && end_ == capacity_ * chunk_size - 1
    {
      for (int i = 0; i < capacity_; i++) {
        temp_chunk[i].data = chunk_[i].data;
        chunk_[i].data = nullptr;
      }
      for (int i = capacity_; i < 2 * capacity_; i++)
        temp_chunk[i].data =
            static_cast<T *>(operator new[](chunk_size * sizeof(T)));
    } else {
      int begin_index = begin_ / chunk_size;
      int end_index = end_ / chunk_size;
      if (begin_index != end_index) {
        for (int i = 0; i < capacity_; i++)
          temp_chunk[i].data = chunk_[(i + begin_index) % capacity_].data;
        for (int i = 0; i < capacity_; i++)
          chunk_[i].data = nullptr;
        for (int i = capacity_; i < 2 * capacity_; i++)
          temp_chunk[i].data =
              static_cast<T *>(operator new[](chunk_size * sizeof(T)));
        begin_ = 0;
        end_ = capacity_ * chunk_size - 1;
      } else {
        temp_chunk[0].data =
            static_cast<T *>(operator new[](chunk_size * sizeof(T)));
        for (int i = begin_ % chunk_size; i < chunk_size; i++)
          new (&temp_chunk[0].data[i]) T(chunk_[begin_index].data[i]);
        for (int i = 1; i < capacity_; i++)
          temp_chunk[i].data = chunk_[(i + begin_index) % capacity_].data;
        temp_chunk[capacity_].data =
            static_cast<T *>(operator new[](chunk_size * sizeof(T)));
        for (int i = 0; i < end_ % chunk_size; i++)
          new (&temp_chunk[capacity_].data[i]) T(chunk_[end_index].data[i]);
        for (int i = capacity_ + 1; i < 2 * capacity_; i++)
          temp_chunk[i].data =
              static_cast<T *>(operator new[](chunk_size * sizeof(T)));
        for (int i = 0; i < chunk_size; i++)
          if (i != end_ % chunk_size)
            chunk_[begin_index].data[i].~T();
        operator delete[](chunk_[begin_index].data);
        for (int i = 0; i < capacity_; i++)
          chunk_[i].data = nullptr;
        begin_ = begin_ % chunk_size;
        end_ = capacity_ * chunk_size + end_ % chunk_size;
      }
    }
    delete[] chunk_;
    chunk_ = temp_chunk;
    capacity_ <<= 1;
  }

public:
  /**
   * TODO Constructors
   */
  deque() {
    chunk_ = new Chunk[initial_size];
    for (int i = 0; i < initial_size; i++)
      chunk_[i].data = static_cast<T *>(operator new[](chunk_size * sizeof(T)));
    begin_ = 0;
    end_ = 0;
    size_ = 0;
    capacity_ = initial_size;
  }
  deque(const deque &other) {
    chunk_ = new Chunk[other.capacity_];
    end_ = other.end_;
    begin_ = other.begin_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    for (int i = 0; i < capacity_; i++)
      chunk_[i].data = static_cast<T *>(operator new[](chunk_size * sizeof(T)));
    if (end_ >= begin_)
      for (int i = begin_; i < end_; i++)
        new (chunk_[i / chunk_size].data + i % chunk_size)
            T(other.chunk_[i / chunk_size].data[i % chunk_size]);
    else
      for (int i = begin_; i < end_ + capacity_ * chunk_size; i++)
        new (chunk_[(i / chunk_size) % capacity_].data + i % chunk_size)
            T(other.chunk_[(i / chunk_size) % capacity_].data[i % chunk_size]);
  }
  /**
   * TODO Deconstructor
   */
  ~deque() {
    if (end_ >= begin_)
      for (int i = begin_; i < end_; i++)
        chunk_[i / chunk_size].data[i % chunk_size].~T();
    else
      for (int i = begin_; i < end_ + capacity_ * chunk_size; i++)
        chunk_[(i / chunk_size) % capacity_].data[i % chunk_size].~T();
    for (int i = 0; i < capacity_; i++)
      operator delete[](chunk_[i].data);
    delete[] chunk_;
  }
  /**
   * TODO assignment operator
   */
  deque &operator=(const deque &other) {
    if (this == &other)
      return *this;
    if (end_ >= begin_)
      for (int i = begin_; i < end_; i++)
        chunk_[i / chunk_size].data[i % chunk_size].~T();
    else
      for (int i = begin_; i < end_ + capacity_ * chunk_size; i++)
        chunk_[(i / chunk_size) % capacity_].data[i % chunk_size].~T();
    for (int i = 0; i < capacity_; i++)
      operator delete[](chunk_[i].data);
    delete[] chunk_;

    chunk_ = new Chunk[other.capacity_];
    end_ = other.end_;
    begin_ = other.begin_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    for (int i = 0; i < capacity_; i++)
      chunk_[i].data = static_cast<T *>(operator new[](chunk_size * sizeof(T)));
    if (end_ >= begin_)
      for (int i = begin_; i < end_; i++)
        new (chunk_[i / chunk_size].data + i % chunk_size)
            T(other.chunk_[i / chunk_size].data[i % chunk_size]);
    else
      for (int i = begin_; i < end_ + capacity_ * chunk_size; i++)
        new (chunk_[(i / chunk_size) % capacity_].data + i % chunk_size)
            T(other.chunk_[(i / chunk_size) % capacity_].data[i % chunk_size]);
    return *this;
  }
  /**
   * access specified element with bounds checking
   * throw index_out_of_bound if out of bound.
   */
  T &at(const size_t &pos) {
    if (empty() || pos >= size())
      throw index_out_of_bound();
    return chunk_[((begin_ + pos) / chunk_size) % capacity_]
        .data[(begin_ + pos) % chunk_size];
  }
  const T &at(const size_t &pos) const {
    if (empty() || pos >= size())
      throw index_out_of_bound();
    return chunk_[((begin_ + pos) / chunk_size) % capacity_]
        .data[(begin_ + pos) % chunk_size];
  }
  T &operator[](const size_t &pos) {
    if (empty() || pos >= size())
      throw index_out_of_bound();
    return chunk_[((begin_ + pos) / chunk_size) % capacity_]
        .data[(begin_ + pos) % chunk_size];
  }
  const T &operator[](const size_t &pos) const {
    if (empty() || pos >= size())
      throw index_out_of_bound();
    return chunk_[((begin_ + pos) / chunk_size) % capacity_]
        .data[(begin_ + pos) % chunk_size];
  }
  /**
   * access the first element
   * throw container_is_empty when the container is empty.
   */
  const T &front() const {
    if (empty())
      throw container_is_empty();
    return chunk_[begin_ / chunk_size].data[begin_ % chunk_size];
  }
  /**
   * access the last element
   * throw container_is_empty when the container is empty.
   */
  const T &back() const {
    if (empty())
      throw container_is_empty();
    int temp_end =
        (end_ - 1 + chunk_size * capacity_) % (chunk_size * capacity_);
    return chunk_[temp_end / chunk_size].data[temp_end % chunk_size];
  }
  /**
   * returns an iterator to the beginning.
   */
  iterator begin() { return iterator(this, 0); }
  const_iterator cbegin() const { return const_iterator(this, 0); }
  /**
   * returns an iterator to the end.
   */
  iterator end() { return iterator(this, size()); }
  const_iterator cend() const { return const_iterator(this, size()); }
  /**
   * checks whether the container is empty.
   */
  bool empty() const { return begin_ == end_; }
  /**
   * returns the number of elements
   */
  size_t size() const { return size_; }
  /**
   * clears the contents
   */
  void clear() {
    if (!empty()) {
      if (begin_ <= end_)
        for (int i = begin_; i < end_; i++)
          chunk_[i / chunk_size].data[i % chunk_size].~T();
      else
        for (int i = begin_; i < end_ + chunk_size * capacity_; i++)
          chunk_[(i / chunk_size) % capacity_].data[i % chunk_size].~T();
    }
    begin_ = end_ = size_ = 0;
  }
  /**
   * inserts elements at the specified location in the container.
   * inserts value before pos
   * returns an iterator pointing to the inserted value
   *     throw if the iterator is invalid or it point to a wrong place.
   */
  iterator insert(iterator pos, const T &value) {
    if (pos.deq_ != this || pos.location_ < 0 || pos.location_ > size())
      throw invalid_iterator();
    if (pos == begin()) {
      push_front(value);
      return begin();
    }
    if (pos == end()) {
      push_back(value);
      return pos;
    }
    if (pos.location_ >= size_ / 2) {
      new (&chunk_[end_ / chunk_size].data[end_ % chunk_size]) T(*(end() - 1));
      for (iterator it = end() - 1; it != pos; it--)
        *it = std::move(*(it - 1));
      *pos = value;
      end_ = (end_ + 1) % (chunk_size * capacity_);
    } else {
      int new_begin =
          (begin_ - 1 + chunk_size * capacity_) % (chunk_size * capacity_);
      new (&chunk_[new_begin / chunk_size].data[new_begin % chunk_size])
          T(*begin());
      iterator temp = pos - 1;
      for (iterator it = begin(); it != temp; it++)
        *it = std::move(*(it + 1));
      *temp = value;
      begin_ = new_begin;
    }
    size_++;
    if (full())
      doubleCapacity();
    return pos;
  }
  /**
   * removes specified element at pos.
   * removes the element at pos.
   * returns an iterator pointing to the following element, if pos pointing to
   * the last element, end() will be returned. throw if the container is empty,
   * the iterator is invalid or it points to a wrong place.
   */
  iterator erase(iterator pos) {
    if (empty() || pos.deq_ != this || pos.location_ < 0 ||
        pos.location_ >= size())
      throw invalid_iterator();
    if (pos.location_ >= size_ / 2) {
      iterator it_end = end() - 1;
      for (iterator it = pos; it != it_end; it++)
        *it = std::move(*(it + 1));
      (*it_end).~T();
      end_ = (end_ - 1 + chunk_size * capacity_) % (chunk_size * capacity_);
    } else {
      for (iterator it = pos; it != begin(); it--)
        *it = std::move(*(it - 1));
      chunk_[begin_ / chunk_size].data[begin_ % chunk_size].~T();
      begin_ = (begin_ + 1) % (chunk_size * capacity_);
    }
    size_--;
    return pos;
  }
  /**
   * adds an element to the end
   */
  void push_back(const T &value) {
    new (&chunk_[end_ / chunk_size].data[end_ % chunk_size]) T(value);
    end_ = (end_ + 1) % (chunk_size * capacity_);
    size_++;
    if (full())
      doubleCapacity();
  }
  /**
   * removes the last element
   *     throw when the container is empty.
   */
  void pop_back() {
    if (empty())
      throw container_is_empty();
    end_ = (end_ - 1 + chunk_size * capacity_) % (chunk_size * capacity_);
    size_--;
    chunk_[end_ / chunk_size].data[end_ % chunk_size].~T();
  }
  /**
   * inserts an element to the beginning.
   */
  void push_front(const T &value) {
    begin_ = (begin_ - 1 + chunk_size * capacity_) % (chunk_size * capacity_);
    new (&chunk_[begin_ / chunk_size].data[begin_ % chunk_size]) T(value);
    size_++;
    if (full())
      doubleCapacity();
  }
  /**
   * removes the first element.
   *     throw when the container is empty.
   */
  void pop_front() {
    if (empty())
      throw container_is_empty();
    chunk_[begin_ / chunk_size].data[begin_ % chunk_size].~T();
    size_--;
    begin_ = (begin_ + 1) % (chunk_size * capacity_);
  }
};

} // namespace sjtu