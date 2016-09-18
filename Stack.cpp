#pragma once

#include <iostream>

using namespace std;

template <typename T>
class stack
{
public:
	stack();
	size_t count() const;
	void push(T const &);
	T pop();
	~stack();
private:
	T * array_;
	size_t array_size_;
	size_t count_;
};

template <typename T>
stack<T>::stack() : array_(nullptr), array_size_(0), count_(0){};

template <typename T>
stack<T>::~stack()
{
	delete[] array_;
}

template <typename T>
size_t stack<T>::count() const
{ 
	return count_; 
}

template <typename T>
void stack<T>::push(T const & x)
{
	if (array_ == nullptr)
	{
		array_ = new T[1];
		array_[0] = x;
		count_++;
		array_size_++;
	}
	else
	{
		if (count_ == array_size_)
		{
			T *p = array_;
			array_ = new T[array_size_ * 2];
			copy(p, p + count_, array_);
			delete[]p;
			array_size_ *= 2;
		}
		array_[count_] = a;
		count_++;
	}
}

template <typename T>
T stack<T>::pop()
{
	if (count_> 0) 
	{
		count_--;
		return array_[count_];
	}
}


