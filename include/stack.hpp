#include <iostream>
#include <stdexcept>


//__________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________


template<typename T>
class allocator 
{
protected:
	allocator(size_t size = 0); /*strong*/
	~allocator(); /*noexcept*/
	auto swap(allocator& other)->void; /*noexcept*/
	allocator(allocator const &) = delete; /*strong*/ // конструктор копирования, запрещен
	auto operator=(allocator const &)->allocator & = delete; // оператор присваивания, запрещен
	T * ptr_;
	size_t size_;
	size_t count_;
};


template <typename T1, typename T2>                 // инициализирует память элементов, на который ссылается указатель
auto construct(T1 * ptr, T2 const & value)->void {  // ptr, списком аргументов value
	new(ptr) T1(value);
}

template <typename T>           // уничтожает объект, на который ссылается указатель ptr, без освобождения
void destroy(T * ptr) noexcept  // памяти, вызывается деструктор объекта
{
	ptr->~T();
}

template <typename FwdIter>                         // уничтожает часть объекта без освобождения памяти, вызывается destroy
void destroy(FwdIter first, FwdIter last) noexcept  
{
	for (; first != last; ++first) {
		destroy(&*first);
	}
}

template <typename T> // создание объекта, выделение памяти глобальной операцией new
allocator<T>::allocator(size_t size) : ptr_(static_cast<T *>(size == 0 ? nullptr : operator new(size * sizeof(T)))),
size_(size), count_(0) {
}

template <typename T> // деструктор
allocator<T>::~allocator() {
operator delete(ptr_); // освобождение области памяти с помощью глобальной операции delete
}

template <typename T> // обмен значений 2 аргументов
auto allocator<T>::swap(allocator& other)->void {
	std::swap(ptr_, other.ptr_);
	std::swap(size_, other.size_);
	std::swap(count_, other.count_);
}

//__________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________
 
template <typename T>
class stack : private allocator<T>
{
public:
	stack(size_t size=0);/*noexcept*/ 
	stack(stack const &); /*strong*/
	auto count() const noexcept->size_t;/*noexcept*/
	auto push(T const &)->void;/*strong*/
	auto pop()->void;/*strong*/
	auto top() const->const T&;/*strong*/
	~stack(); 	/*noexcept*/
	auto operator=(const stack&tmp)->stack&;/*strong*/
	auto empty() const noexcept->bool;/*noexcept*/

};


template<typename T> 
auto stack<T>::empty() const noexcept->bool {
	return (allocator<T>::count_ == 0);
}

template <typename T>
stack<T>::~stack() {
destroy(allocator<T>::ptr_, allocator<T>::ptr_ + allocator<T>::count_);};

template <typename T>
stack<T>::stack(size_t size): allocator<T>(size) {};



template <typename T>
auto stack<T>::push(T const &val)->void {
	if (allocator<T>::count_ == allocator<T>::size_) {
		size_t array_size = allocator<T>::size_ * 2 + (allocator<T>::size_ == 0);
		stack<T> temp(array_size);
		while (temp.count() < allocator<T>::count_) temp.push(allocator<T>::ptr_[temp.count()]); 
		this->swap(temp);
	}
	construct(allocator<T>::ptr_ + allocator<T>::count_, val);
	++allocator<T>::count_;
}


template <typename T>
stack<T>::stack(stack const &tmp): allocator<T>(tmp.size_){
	for (size_t i = 0; i < tmp.count_; i++) construct(allocator<T>::ptr_ + i, tmp.ptr_[i]);
	allocator<T>::count_ = tmp.count_;
};

template <typename T>
auto stack<T>::operator=(const stack &tmp)->stack& {
	if (this != &tmp) {
		(stack(tmp)).swap(*this);
	}
	return *this;
}


template <typename T>
auto stack<T>::count() const noexcept->size_t {
	return allocator<T>::count_;
}

template <typename T>
auto stack<T>::pop()->void {
	if (allocator<T>::count_ == 0) throw std::logic_error("Empty!");
	destroy(allocator<T>::ptr_+allocator<T>::count_);
	--allocator<T>::count_;
}

template <typename T>
auto stack<T>::top() const->const T& {
	if (allocator<T>::count_ == 0) throw std::logic_error("Empty!");
	return allocator<T>::ptr_[allocator<T>::count_-1];

}
