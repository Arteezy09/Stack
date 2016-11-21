#include <iostream>
#include <new>  
#include <stdexcept>
#include <vector>
#include <memory>


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

bitset::bitset(size_t size) : ptr_(std::make_unique<bool[]>(size)),
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
	~allocator() /*noexcept*/; // освобождене области памяти

	auto resize() /*strong*/ -> void; // увеличение памяти

	auto construct(T * ptr, T const & value) /*strong*/ -> void; // инициализирует память элементов, на который ссылается указатель
	                                                             // ptr, списком аргументов value
	auto destroy(T * ptr) /*noexcept*/ -> void; // уничтожает объект, на который ссылается указатель ptr, без освобождения
                                                    // памяти, вызывается деструктор объекта
	auto get() /*noexcept*/ -> T *;
	auto get() const /*noexcept*/ -> T const *;

	auto count() const /*noexcept*/ -> size_t;
	auto full() const /*noexcept*/ -> bool; 
	auto empty() const /*noexcept*/ -> bool; 
	auto swap(allocator & other) /*noexcept*/ -> void; // обмен значений 2 аргументов
private:
	auto destroy(T * first, T * last) /*noexcept*/ -> void; // уничтожает часть объекта, вызывается destroy, а после деструктор
	
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


template <typename T>             // уничтожается часть объекта, вызывается destroy, а дальше явно вызывается деструктор            
auto allocator<T>::destroy(T * first, T * last)->void
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
	explicit // explicit используется для создания явных конструкторов
	stack(size_t size = 0);/*strong*/
	auto operator =(stack const & other) /*strong*/ -> stack &;
	stack (stack const & other) =default;/*strong*/
	auto empty() const /*noexcept*/ -> bool;
	auto count() const /*noexcept*/ -> size_t;

	auto push(T const & value) /*strong*/ -> void;
	auto pop() /*strong*/ -> void;
	auto top() /*strong*/ -> T &;
	auto top() const /*strong*/ -> T const &;

private:
	allocator<T> allocate;

	//auto throw_is_empty() const -> void;
};


/*template<typename T>
auto stack<T>::throw_is_empty() const -> void {
	throw std::logic_error("Error!"); 
}*/

template<typename T>
auto stack<T>::empty() const->bool {
	return (allocate.count() == 0);
}


template <typename T>
stack<T>::stack(size_t size) : allocate(size) {};


template <typename T>
auto stack<T>::push(T const &val)->void {
	if (allocate.full()) {
		allocate.resize();
	}
	allocate.construct(allocate.get() + allocate.count(), val);
}


template <typename T>
auto stack<T>::operator=(const stack &tmp)->stack&  {
	if (this != &tmp) {
		stack(tmp).allocate.swap(allocate);
	}
	return *this;
}


template <typename T>
auto stack<T>::count() const->size_t {
	return allocate.count();
}


template <typename T>
auto stack<T>::pop()->void {
	if (allocate.count() == 0) throw std::logic_error("Empty!");
	allocate.destroy(allocate.get() + (this->count()-1));
}


template <typename T>
auto stack<T>::top() const->const T&{
	if (allocate.count() == 0) throw std::logic_error("Empty!");
return(*(allocate.get() + this->count() - 1));

}


template <typename T>
auto stack<T>::top()->T&{
	if (allocate.count() == 0) throw std::logic_error("Empty!");
return(*(allocate.get() + this->count() - 1));
}
