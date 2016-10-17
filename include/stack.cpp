#pragma once
#include <iostream>
using namespace std;


template <typename T>
class allocator
{
protected:
    allocator(size_t size = 0);
    ~allocator();
    auto swap(allocator & other) -> void;
	
    auto construct(T*ptr,T const & val) -> void;
    auto destroy(T *ptr) -> void;
    auto destroy(T *first, T *last) -> void;
    
    allocator(allocator const &) = delete;
    auto operator =(allocator const &) -> allocator & = delete;
    
    T * ptr_;
    size_t size_;
    size_t count_;
};

template <typename T>
allocator<T>::allocator(size_t size):count_(0), size_(size)
{
	if (size==0)
	{
		ptr_=nullptr;
	}
	else
	{
		ptr_ = static_cast<T*>(operator new(size*sizeof(T)));
	}
}

template<typename T> 
allocator<T>::~allocator() 
{
	destroy(ptr_, ptr_ + size_);
	operator delete(ptr_);
}

template <typename T>
void allocator<T>::construct(T *ptr,T const & val)
{
	new(ptr) T(val);
}

template <typename T>
void allocator<T>::destroy(T *ptr)
{
	ptr->~T();
}

template <typename T>
void allocator<T>::destroy(T *first,T *last)
{
	for (; first != last; first++)
	{
		destroy(first);
	}
}
	
template<typename T> /*noexcept*/
auto allocator<T>::swap(allocator & other) -> void 
{
	std::swap(ptr_, other.ptr_);
	std::swap(count_, other.count_);
	std::swap(size_, other.size_);
}


template <typename T>
class stack : private allocator<T>
{
public:
	stack();                                   /* noexcept */
	stack(const stack &);                      /* strong */
	~stack();                                  /* noexcept */

	auto count() const noexcept->size_t;       /* noexcept */
	auto empty() const->bool;                  /* noexcept */

	auto operator=(const stack &)->stack &;    /* strong */
	auto push(T const &)->void;                /* strong */
	auto top() const->T &;                     /* strong */
	auto pop()->T;                             /* strong */
};


template <typename T>
stack<T>::stack() : allocator<T>() {}


template <typename T>
stack<T>::~stack() {}


template<typename T>
auto stack<T>::empty() const->bool
{ 
	return(allocator<T>::count_ == 0); 
}


template <typename T>
auto stack<T>::count() const noexcept->size_t 
{ 
	return allocator<T>::count_; 
}


template<typename T>         /* strong */
auto new_copy(const T * rhs, size_t count__, size_t array_size__)->T* 
{
	T* ptr = new T[array_size__];
	try 
	{ 
	        std::copy(rhs, rhs + count__, ptr); 
	}
	catch(...) 
	{
		delete[] ptr;
		throw;
	}
	return ptr;
}


template <typename T>
stack<T>::stack(const stack & rhs) : allocator<T>(rhs.size_) 
{
	for (size_t i = 0; i < rhs.count_; i++)
	{
		allocator<T>::construct(allocator<T>::ptr_ + i, rhs.ptr_[i]);
	}
	allocator<T>::count_ = rhs.count_;
}


template <typename T>
auto stack<T>::push(T const & value)->void
{
	if (allocator<T>::size_ == allocator<T>::count_)
	{
		int size = allocator<T>::size_ * 2 + (allocator<T>::size_ == 0);
		T * ptr = new_copy(allocator<T>::array_, allocator<T>::count_, size);
		delete[] allocator<T>::array_;
		allocator<T>::array_ = ptr;
		allocator<T>::size_ = size;
	}
	allocator<T>::array_[allocator<T>::count_] = value;
	++allocator<T>::count_;
}


template <typename T>
auto stack<T>::top() const->T & 
{
	if (allocator<T>::count_ == 0) 
	{
		throw std::logic_error("The stack is Empty");
	}
	return allocator<T>::array_[allocator<T>::count_ - 1];
}


template <typename T>
auto stack<T>::pop()->T 
{
	if (allocator<T>::count_ == 0) 
	{
		throw std::logic_error("The stack is Empty");
	}
	return allocator<T>::array_[--allocator<T>::count_];
}


template<typename T>
auto stack<T>::operator=(const stack & rhs)->stack &
{
	if (this != & rhs) 
	{
		(stack(rhs)).swap(*this);
	}
	return *this;		
}

/*
int main()
{
stack<int> st;
system("pause");
return 0;
}
*/
