#pragma once
#include <iostream>
using namespace std;

template <typename T1, typename T2>
void construct(T1 * ptr, T2 const & value) {
	new(ptr) T1 (value);
}

template <typename T>
void destroy(T * ptr) noexcept {
	ptr->~T();
}

template <typename FwdIter>
void destroy(FwdIter first, FwdIter last) noexcept {
	for (; first != last; ++first) {
		destroy(&*first);
	}
}

template <typename T>
class allocator
{
protected:
    allocator(size_t size = 0);
    ~allocator();
    auto swap(allocator & other) -> void;
	
    allocator(allocator const &) = delete;
    auto operator =(allocator const &) -> allocator & = delete;
    
    T * ptr_;
    size_t size_;
    size_t count_;
};

template <typename T>
allocator<T>::allocator(size_t size):count_(0), size_(size) {
	if (size==0) { ptr_=nullptr; }
	else 
	{ 
		ptr_ = static_cast<T*>(operator new(size*sizeof(T))); 
	}
}

template<typename T> 
allocator<T>::~allocator() {
	destroy(ptr_, ptr_ + size_);
	operator delete(ptr_);
}
	
template<typename T>                               /*noexcept*/
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
auto stack<T>::empty() const->bool { 
	return(allocator<T>::count_ == 0); 
}


template <typename T>
auto stack<T>::count() const noexcept->size_t { 
	return allocator<T>::count_; 
}


template<typename T>                              /* strong */
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
		construct(allocator<T>::ptr_ + i, rhs.ptr_[i]);
	}
	allocator<T>::count_ = rhs.count_;
}


template <typename T>
auto stack<T>::push(T const & value)->void
{
	if (allocator<T>::size_ == allocator<T>::count_)
	{
		size_t size = allocator<T>::size_ * 2 + (allocator<T>::size_ == 0);
		T * ptr = new_copy(allocator<T>::ptr_, allocator<T>::count_, size);
		delete[] allocator<T>::ptr_;
		allocator<T>::ptr_ = ptr;
		allocator<T>::size_ = size;
	}
	allocator<T>::ptr_[allocator<T>::count_] = value;
	++allocator<T>::count_;
}


template <typename T>
auto stack<T>::top() const->T & 
{
	if (allocator<T>::count_ == 0) 
	{
		throw std::logic_error("The stack is Empty");
	}
	return allocator<T>::ptr_[allocator<T>::count_ - 1];
}


template <typename T>
auto stack<T>::pop()->T 
{
	if (allocator<T>::count_ == 0) 
	{
		throw std::logic_error("The stack is Empty");
	}
	return allocator<T>::ptr_[--allocator<T>::count_];
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
