#include <iostream>
#include <new>  
#include <stdexcept>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include <thread>


//_________________________________________________________________________________________________________________________________________
//_________________________________________________________________________________________________________________________________________


class bitset
{
public:
	explicit // explicit используется для создания явных конструкторов
	bitset(size_t size) /*strong*/;  // конструктор c параметром

	bitset(bitset const & other) = delete; // конструктор копирования, запрещен
	auto operator =(bitset const & other)->bitset & = delete; // оператор присваивания, запрещен
	bitset(bitset && other) = delete; // конструктор перемещения, запрещен
	auto operator =(bitset && other)->bitset & = delete; // оператор присваивания перемещения, запрещен


	auto set(size_t index) /*strong*/ -> void; // инициализация битов единицами
	auto reset(size_t index) /*strong*/ -> void; // обнуление битов
	auto test(size_t index) /*strong*/ -> bool; // возврат значения бита объекта bitset
	auto counter() /*noexcept*/ -> size_t; // возврат количество битов, равных 1
	auto size() /*noexcept*/ -> size_t; // возврат размер объекта bitset (количество битов)

private:
	std::unique_ptr<bool[]>  ptr_; // умный указатель, который не может быть скопирован или задан через операцию присвоения,
	size_t size_;                  // но он может быть передан другому unique_ptr
	size_t counter_;
};

bitset::bitset(size_t size) : ptr_(std::make_unique<bool[]>(size)), // создание объекта размером size, состоящий из всех нулей 
size_(size), counter_(0) {
}


auto bitset::set(size_t index) -> void { // инициализация битов единицами
	if (index < size_)                
	{ 
		ptr_[index] = true; 
		++counter_; 
	} 
        else throw("Error!"); 
}


auto bitset::reset(size_t index) -> void { // обнуление битов
	if (index < size_) 
	{ 
		ptr_[index] = false; 
		--counter_; 
	} 
	else throw("Error!"); 
}


auto bitset::test(size_t index) -> bool { // возврат значения бита объекта bitset
	if (index < size_) 
		return ptr_[index]; 
	else throw("Error!"); 
}


auto bitset::size() -> size_t { return size_; } // возврат размер объекта bitset (количество битов)


auto bitset::counter() -> size_t { return counter_; } // возврат количество битов, равных 1


//__________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________


template <typename T>
class allocator
{
public:
	explicit // explicit используется для создания явных конструкторов
	allocator(std::size_t size = 0) /*strong*/;
	allocator(allocator const & other) /*strong*/; // конструктор копирования
	auto operator =(allocator const & other)->allocator & = delete; // оператор присваивания, запрещен
	~allocator() /*noexcept*/; // освобождение области памяти

	auto resize() /*strong*/ -> void; // увеличение памяти

	auto construct(T * ptr, T const & value) /*strong*/ -> void; // инициализирует память элементов, на который ссылается указатель
	                                                             // ptr, списком аргументов value
	auto destroy(T * ptr) /*noexcept*/ -> void; // уничтожает объект, на который ссылается указатель ptr, без освобождения
                                                    // памяти, вызывается деструктор объекта
	
	auto get() /*noexcept*/ -> T *; // получение ptr_
	auto get() const /*noexcept*/ -> T const *; // получение ptr_ (const метод)

	auto count() const /*noexcept*/ -> size_t; 
	auto full() const /*noexcept*/ -> bool; // полный?
	auto empty() const /*noexcept*/ -> bool; // пустой?
	auto swap(allocator & other) /*noexcept*/ -> void; // обмен значений 2 аргументов
private:
	auto destroy(T * first, T * last) /*noexcept*/ -> void; // уничтожается часть объекта, для этого диапазона вызывается destroy, 
	                                                        // а дальше явно вызывается деструктор объекта
	size_t size_;
	T * ptr_;
	std::unique_ptr<bitset> map_;
};


template <typename T> // создание объекта, выделение памяти глобальной операцией new
allocator<T>::allocator(size_t size) : ptr_(static_cast<T *>(size == 0 ? nullptr : operator new(size * sizeof(T)))), 
size_(size), map_(std::make_unique<bitset>(size)){
}

template<typename T> // копирует объект распределителя памяти так,что область памяти, выделенная оригиналом или копией, может быть  
allocator<T>::allocator(allocator const & tmp) : allocator<T>(tmp.size_){ // удалена одним из них
	for (size_t i = 0; i < size_; ++i) {
		construct(ptr_ + i, tmp.ptr_[i]);
	}
}

template <typename T> // деструктор
allocator<T>::~allocator() {
	if (map_->counter() > 0) {
		destroy(ptr_, ptr_ + map_->size());
	}
	operator delete(ptr_); // освобождение области памяти с помощью глобальной операции delete
}

template <typename T> // обмен значений 2 аргументов
auto allocator<T>::swap(allocator & other)->void { 
	std::swap(ptr_, other.ptr_);
	std::swap(size_, other.size_);
	std::swap(map_, other.map_);
}

template <typename T>                                          // инициализирует память элементов, на который ссылается указатель
auto allocator<T>::construct(T * ptr, T const & value)->void { // ptr, списком аргументов value
	if (ptr < ptr_ || ptr >= ptr_ + size_) {               
		throw std::out_of_range("Error");
	}
	new(ptr) T(value);
	map_->set(ptr - ptr_);
}

template <typename T>                     // уничтожает объект, на который ссылается указатель ptr, без освобождения
auto allocator<T>::destroy(T * ptr)->void // памяти, вызывается деструктор объекта
{
	ptr->~T();
	map_->reset(ptr - ptr_);	
}


template <typename T>                                  // уничтожается часть объекта, для этого диапазона вызывается destroy,             
auto allocator<T>::destroy(T * first, T * last)->void  // а дальше явно вызывается деструктор объекта
{
	for (; first != last; ++first) {
		destroy(&*first);
	}
}

template<typename T> // увеличение памяти
auto allocator<T>::resize()-> void {
	size_t size = size_ * 2 + (size_ == 0);
	allocator<T> buff(size);
	for (size_t i = 0; i < size_; ++i) {
	if (map_->test(i)) 
	{ buff.construct(buff.ptr_ + i, ptr_[i]); }
	}
	this->swap(buff);
}

template<typename T>  
auto allocator<T>::empty() const -> bool { return (map_->counter() == 0); }

template<typename T>  
auto allocator<T>::full() const -> bool { return (map_->counter() == size_); }

template<typename T>  
auto allocator<T>::get() -> T * { return ptr_; }

template<typename T>
auto allocator<T>::get() const -> T const * { return ptr_; }

template<typename T> 
auto allocator<T>::count() const -> size_t { return map_->counter(); }


//__________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________



template <typename T>
class stack
{
public:
	explicit
	stack(size_t size = 0);
	stack(stack const & other); /*strong*/
	auto operator =(stack const & other) /*strong*/ -> stack &;
	

	auto empty() const /*noexcept*/ -> bool;
	auto count() const /*noexcept*/ -> size_t;

	auto push(T const & value) /*strong*/ -> void;
	auto pop() /*strong*/ -> void;
	auto top() /*strong*/ -> T &;
	auto top() const /*strong*/ -> T const &;

private:
	allocator<T> allocator_;
        mutable std::mutex mutexstack_;
	auto throw_is_empty() const -> void;              /*strong*/
};


template<typename T>
auto stack<T>::throw_is_empty()const->void
{
	throw std::logic_error("Error!"); 
}

template <typename T>/*noexcept*/
stack<T>::stack(size_t size) : allocator_(size)
{}



template <typename T>
auto stack<T>::operator=(const stack &st)-> stack &/*strong*/
{
        std::lock(mutexstack_,st.mutexstack_);
	std::lock_guard<std::mutex> lock_a(mutexstack_, std::adopt_lock);		
 	std::lock_guard<std::mutex> locker(st.mutexstack_, std::adopt_lock);
	if (this != &st)
	{
		(allocator<T>(st.allocator_)).swap(this->allocator_);
	}
	return *this;
}

template <typename T>
size_t  stack<T>::count() const/*noexcept*/
{
	std::lock_guard<std::mutex> locker(mutexstack_);
	return allocator_.count();
}

template <typename T>
void stack<T>::push(T const &value)/*strong*/
{
	std::lock_guard<std::mutex> locker(mutexstack_);
	if (allocator_.full())
		allocator_.resize();
	allocator_.construct(allocator_.get() + allocator_.count(), value);
}

template <typename T>
void stack<T>::pop()/*strong*/
{
	std::lock_guard<std::mutex> locker(mutexstack_);
	if (allocator_.count() == 0)
	{
		this->throw_is_empty();
	}
	allocator_.destroy(allocator_.get() + (allocator_.count()-1));
}

template <typename T>
auto stack<T>::top()-> T&/*strong*/
{
	std::lock_guard<std::mutex> locker(mutexstack_);
	if (allocator_.count() > 0) 
		return(*(allocator_.get() + allocator_.count() - 1));
	else this->throw_is_empty();
}

template<typename T>
auto stack<T>::top()const->T const & 
{
	std::lock_guard<std::mutex> locker(mutexstack_);
	if (allocator_.count() > 0) 
		return(*(allocator_.get() + allocator_.count() - 1));
	else this->throw_is_empty();
}

template <typename T>/*noexcept*/
auto stack<T>::empty()const->bool 
{
	std::lock_guard<std::mutex> locker(mutexstack_);
	return(allocator_.empty() == 1);
}


