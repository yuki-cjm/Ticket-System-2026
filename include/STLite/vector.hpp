#pragma once

#include "utils/Error.hpp"


namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector
{
  public:
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	class const_iterator;
	class iterator
	{
	// The following code is written for the C++ type_traits library.
	// Type traits is a C++ feature for describing certain properties of a type.
	// For instance, for an iterator, iterator::value_type is the type that the
	// iterator points to.
	// STL algorithms and containers may use these type_traits (e.g. the following
	// typedef) to work properly. In particular, without the following code,
	// @code{std::sort(iter, iter1);} would not compile.
	// See these websites for more information:
	// https://en.cppreference.com/w/cpp/header/type_traits
	// About value_type: https://blog.csdn.net/u014299153/article/details/72419713
	// About iterator_category: https://en.cppreference.com/w/cpp/iterator
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
        vector<T>* vec_;
	public:
        T* p_;
        iterator() = default;
        iterator(vector<T>* vec, T* p) : vec_(vec), p_(p) {};
        iterator(iterator &other) : vec_(other.vec_), p_(other.p_) {};
        iterator(const iterator &other) : vec_(other.vec_), p_(other.p_) {};
		/**
		 * return a new iterator which pointer n-next elements
		 * as well as operator-
		 */
		iterator operator+(const int &n) const
		{
			//TODO
            iterator temp(*this);
            temp.p_ = temp.p_ + n;
            return temp;
		}
		iterator operator-(const int &n) const
		{
			//TODO
            iterator temp(*this);
            temp.p_ = temp.p_ - n;
            return temp;
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const
		{
			//TODO
            if(this->vec_ != rhs->vec)
                throw invalid_iterator();
            return this->p_ - rhs.p_;
		}
		iterator& operator+=(const int &n)
		{
			//TODO
            this->p_ += n;
            return *this;
		}
		iterator& operator-=(const int &n)
		{
			//TODO
            this->p_ -= n;
            return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int)
        {
            iterator temp(*this);
            this->p_++;
            return temp;
        }
		/**
		 * TODO ++iter
		 */
		iterator& operator++()
        {
            this->p_++;
            return *this;
        }
		/**
		 * TODO iter--
		 */
		iterator operator--(int)
        {
            iterator temp(*this);
            this->p_--;
            return temp;
        }
		/**
		 * TODO --iter
		 */
		iterator& operator--()
        {
            this->p_++;
            return *this;
        }
		/**
		 * TODO *it
		 */
		T& operator*() const { return *p_; }
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory address).
		 */
		bool operator==(const iterator &rhs) const { return this->p_ == rhs.p_; }
		bool operator==(const const_iterator &rhs) const { return this->p_ == rhs.p_; }
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const { return this->p_ != rhs.p_; }
		bool operator!=(const const_iterator &rhs) const { return this->p_ != rhs.p_; }
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator
	{
	  public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	  private:
		/*TODO*/
        const vector<T>* vec_;
      public:
        const T* p_;
        const_iterator() = default;
        const_iterator(vector<T>* vec, T* p) : vec_(vec), p_(p) {};
        const_iterator(const_iterator &other) : vec_(other.vec_), p_(other.p_) {};
        const_iterator(const vector<T>* vec, T* p) : vec_(vec), p_(p)  {};
        const_iterator operator+(const int &n) const
		{
            const_iterator temp(*this);
            temp.p_ = temp.p_ + n;
            return temp;
		}
		const_iterator operator-(const int &n) const
		{
            const_iterator temp(*this);
            temp.p_ = temp.p_ - n;
            return temp;
		}
		int operator-(const const_iterator &rhs) const
		{
            if(this->vec_ != rhs->vec_)
                throw invalid_iterator();
            return this->p_ - rhs.p_;
		}
		const_iterator& operator+=(const int &n)
		{
            this->p_ += n;
            return *this;
		}
		const_iterator& operator-=(const int &n)
		{
            this->p_ -= n;
            return *this;
		}
		const_iterator operator++(int)
        {
            const_iterator temp(*this);
            this->p_++;
            return temp;
        }
		const_iterator& operator++()
        {
            this->p_++;
            return *this;
        }
		const_iterator operator--(int)
        {
            const_iterator temp(*this);
            this->p_--;
            return temp;
        }
		const_iterator& operator--()
        {
            this->p_++;
            return *this;
        }
		const T& operator*() const { return *p_; }
		bool operator==(const iterator &rhs) const { return this->p_ == rhs.p_; }
		bool operator==(const const_iterator &rhs) const { return this->p_ == rhs.p_; }
		bool operator!=(const iterator &rhs) const { return this->p_ != rhs.p_; }
		bool operator!=(const const_iterator &rhs) const { return this->p_ != rhs.p_; }
	};
  private:
    size_t size_;
    size_t capacity_;
    T* data_;
    void doubleCapacity()
    {
        T* data = static_cast<T*>(operator new[](2 * capacity_ * sizeof(T)));
        for(size_t i = 0; i < size_; i++)
        {
            new (data + i) T(data_[i]);
            data_[i].~T();
        }
        operator delete[](data_);
        data_ = data;
        capacity_ *= 2;
    }
  public:
	/**
	 * TODO Constructs
	 * At least two: default constructor, copy constructor
	 */
	vector()
    {
        size_ = 0;
        capacity_ = 1;
        data_ = static_cast<T*>(operator new[](capacity_ * sizeof(T)));
    }

    vector(int size, T &data) {
        size_ = size;
        capacity_ = size;
        data_ = static_cast<T*>(operator new[](capacity_ * sizeof(T)));
        for (int i = 0; i < size_; i++) {
            new (data_ + i) T(data);
        }
    }

	vector(const vector &other)
    {
        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = static_cast<T*>(operator new[](capacity_ * sizeof(T)));
        for(size_t i = 0; i < size_; i++)
            new (data_ + i) T(other[i]);
    }
	/**
	 * TODO Destructor
	 */
	~vector()
    {
        for(size_t i = 0; i < size_; i++)
            data_[i].~T();
        operator delete[](data_);
    }
	/**
	 * TODO Assignment operator
	 */
	vector &operator=(const vector &other)
    {
        if(this == &other) return *this;
        for(size_t i = 0; i < size_; i++)
            data_[i].~T();
        operator delete[](data_);
        capacity_ = other.capacity_;
        size_ = other.size_;
        data_ = static_cast<T*>(operator new[](capacity_ * sizeof(T)));
        for(size_t i = 0; i < size_; i++)
            new (data_ + i) T(other.data_[i]);
        return *this;
    }
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T & at(const size_t &pos)
    {
        if(pos >= size_)
            throw index_out_of_bound();
        return data_[pos];
    }
	const T & at(const size_t &pos) const
    {
        if(pos >= size_)
            throw index_out_of_bound();
        return data_[pos];
    }
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos)
    {
        if(pos >= size_)
            throw index_out_of_bound();
        return data_[pos];
    }
	const T & operator[](const size_t &pos) const
    {
        if(pos >= size_)
            throw index_out_of_bound();
        return data_[pos];
    }
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const
    {
        if(size_ == 0)
            throw container_is_empty();
        return data_[0];
    }
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const
    {
        if(size_ == 0)
            throw container_is_empty();
        return data_[size_ - 1];
    }
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() { return iterator(this, data_); }
	const_iterator begin() const { return const_iterator(this, data_); }
	const_iterator cbegin() const { return const_iterator(this, data_); }
	/**
	 * returns an iterator to the end.
	 */
	iterator end(){ return iterator(this, data_ + size_); }
	const_iterator end() const { return const_iterator(this, data_ + size_); }
	const_iterator cend() const { return const_iterator(this, data_ + size_); }
	/**
	 * checks whether the container is empty
	 */
	bool empty() const { return size_ == 0; }
	/**
	 * returns the number of elements
	 */
	size_t size() const { return size_; }
	/**
	 * clears the contents
	 */
	void clear()
    {
        for(size_t i = 0; i < size_; i++)
            data_[i].~T();
        size_ = 0;
    }
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value)
    {
        if(pos == end())
            new (pos.p_) T(value);
        else
        {
            iterator ed = end();
            new (ed.p_) T(*(ed - 1));
            for(iterator it = ed - 1; it != pos; it--)
                *it = *(it - 1);
            *pos = value;
        }
        size_++;
        if(size_ == capacity_)
            doubleCapacity();
        return pos;
    }
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value)
    {
        if(ind > size_)
            throw index_out_of_bound();
        if(size_ == capacity_)
            doubleCapacity();
        if(ind == size_)
            new (data_ + size_) T(value);
        else
        {
            new (data_ + size_) T(data_[size_ - 1]);
            for(size_t i = size_ - 1; i > ind; i--)
                data_[i] = data_[i - 1];
            data_[ind] = value;
        }
        size_++;
        return iterator(this, data_ + ind);
    }
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos)
    {
        for(iterator it = pos; it != end() - 1; it++)
            *it = *(it + 1);
        data_[size_ - 1].~T();
        size_--;
        return pos;
    }
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t &ind)
    {
        if(ind >= size_)
            throw index_out_of_bound();
        for(size_t i = ind; i < size_ - 1; i++)
            data_[i] = data_[i + 1];
        data_[size_ - 1].~T();
        size_--;
        return iterator(this, data_ + ind);
    }
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) { insert(end(), value); }
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back()
    {
        if(size_ == 0)
            throw container_is_empty();
        erase(end() - 1);
    }
};
}