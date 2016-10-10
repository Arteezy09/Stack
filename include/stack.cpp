#pragma once
#include <iostream>
using namespace std;

template <typename T>
class stack
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

	
private:
	T * array_;
	size_t array_size_;
	size_t count_;
};


template <typename T>
stack<T>::stack() : array_(nullptr), array_size_(0), count_(0){}


template <typename T>
stack<T>::~stack() {
	delete[] array_;
}


template<typename T>
auto stack<T>::empty() const->bool { 
	return(count_ == 0); 
}


template <typename T>
auto stack<T>::count() const noexcept->size_t { 
	return count_; 
}


template<typename T>         /* strong */
auto new_copy(const T * rhs, size_t count__, size_t array_size__)->T* {
	T* ptr = new T[array_size__];
	try {
		std::copy(rhs, rhs + count__, ptr);
	}
	catch(...) {
		delete[] ptr;
		throw;
	}
	return ptr;
}


template <typename T>
stack<T>::stack(const stack & rhs) {
	array_size_ = rhs.array_size_;
        count_ = rhs.count_;
        array_ = new_copy(rhs.array_, rhs.count_, rhs.array_size_);
}


template <typename T>
auto stack<T>::push(T const & value)->void {
	if (array_size_ == count_) {
		int size = array_size_ * 2 + (array_size_ == 0);
		T * ptr = new_copy(array_, count_, size);
		delete[] array_;
		array_ = ptr;
		array_size_ = size;
	}
	array_[count_] = value;
	++count_;
}


template <typename T>
auto stack<T>::top() const->T & {
	if (count_ == 0) {
		throw std::logic_error("The stack is Empty");
	}
	return array_[count_ - 1];
}


template <typename T>
auto stack<T>::pop()->T {
	if (count_ == 0) {
		throw std::logic_error("The stack is Empty");
	}
	return array_[--count_];
}


template<typename T>
auto stack<T>::operator=(const stack & rhs)->stack & {
	if (this != & rhs) {
		T *ptr = new_copy(rhs.array_, rhs.count_, rhs.array_size_);
		delete[] array_;
		count_ = rhs.count_;
		array_size_ = rhs.array_size_;
		array_ = ptr;
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
