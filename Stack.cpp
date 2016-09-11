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
stack<T>::stack() : array_(nullptr), array_size_(0), count_(0){}


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
	if (count_ == 0)
	{
		array_ = new T[1];
		array_[count_] = x;
		count_++; 
		array_size_ = 1;
	}
	else
	{
		T *array_2 = array_;
		array_size_ = count_ + 1;
		array_ = new T[array_size_];
		for (int i = 0; i < count_; i++) array_[i] = array_2[i];
		delete[] array_2;
		array_[count_] = x;
		count_++;
	}
}

template <typename T>
T stack<T>::pop()
{
	if (count_ == 0) throw "\n empty stack";
	else
	{
		count_--;
		return array_[count_];
	}
}


int main()
{
	stack<int> a;
	a.push(2);
	a.pop();
	system("pause");
	return 0;
}
