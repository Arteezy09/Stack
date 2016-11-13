#include <cstring>
#include <iostream>
#include <memory>
#include <new>  


template <typename T>
auto new_array(const T * array_, size_t size, size_t new_size) -> T*
{ 
	T * temp = new T[new_size];
	try 
	{
		std::copy(array_, array_ + size, temp);
	}
	catch (...) 
	{
		delete[] temp;
		throw;
	}
	return temp;
}


//_______________________________________________________________________________________________________________________________________
//_______________________________________________________________________________________________________________________________________


class bitset
{
public:
	explicit
	bitset(size_t size)   /*strong*/;
	
	bitset(bitset const & other) = delete;
	auto operator =(bitset const & other) -> bitset & = delete;
	bitset(bitset && other) = delete;
	auto operator =(bitset && other)->bitset & = delete;
	
	auto set(size_t index) -> void;   /*strong*/
	auto reset(size_t index) -> void;   /*strong*/
	auto test(size_t index) -> bool;   /*strong*/
	auto size() -> size_t;   /*noexcept*/
	auto counter() -> size_t;   /*noexcept*/
private:
	std::unique_ptr<bool[]>  ptr_;
	size_t size_;
	size_t counter_;
};


bitset::bitset(size_t size) : ptr_(std::make_unique<bool[]>(size)),
size_(size), counter_(0) {
}


auto bitset::set(size_t index) -> void { 
	if (index < size_)
	{ 
		ptr_[index] = true; 
		++counter_; 
	} 
        else throw("Error!"); 
}


auto bitset::reset(size_t index) -> void { 
	if (index < size_) 
	{ 
		ptr_[index] = false; 
		--counter_; 
	} 
	else throw("Error!"); 
}


auto bitset::test(size_t index) -> bool { 
	if (index < size_) 
		return ptr_[index]; 
	else throw("Error!"); 
}


auto bitset::size() -> size_t { 
	return size_; 
}


auto bitset::counter() -> size_t { 
	return counter_; 
}


//_______________________________________________________________________________________________________________________________________
//_______________________________________________________________________________________________________________________________________


template <typename T>
class allocator
{
public:
	explicit
	allocator(std::size_t size = 0);   /*strong*/
	allocator(allocator const & other);   /*strong*/
	auto operator =(allocator const & other) -> allocator & = delete;
	~allocator();
	auto resize() -> void;   /*strong*/
	auto construct(T * ptr, T const & value) -> void;   /*strong*/
	auto destroy(T * ptr) -> void;   /*noexcept*/
	auto get() -> T *;   /*noexcept*/
	auto get() const -> T const *;   /*noexcept*/
	auto count() const -> size_t;   /*noexcept*/
	auto full() const -> bool;   /*noexcept*/
	auto empty() const -> bool;   /*noexcept*/
	auto swap(allocator & other) -> void;   /*noexcept*/
private:
	auto destroy(T * first, T * last) -> void;   /*strong*/
	
	T * ptr_;
	size_t size_;
	std::unique_ptr<bitset> map_;
};


template <typename T>
allocator<T>::allocator(size_t size) 
	: ptr_(static_cast<T *>(size == 0 ? nullptr : operator new(size * sizeof(T)))), 
        size_(size),
        map_(std::make_unique<bitset>(size)) {
}


template<typename T> 
allocator<T>::~allocator() {
	if (this->count() > 0) {
        allocator<T>::destroy(ptr_, ptr_ + size_); 
	}
	operator delete(ptr_);	
}


template <typename T>
auto allocator<T>::construct(T *ptr,T const & val) -> void
{
	if (ptr >= ptr_ && ptr < ptr_ + size_ && map_->test(ptr - ptr_)){
		new(ptr)T(val);
		map_->set(ptr - ptr_);
	}
	else { throw("Error!"); }
}


template <typename T>
auto allocator<T>::destroy(T *ptr) -> void
{
	if (map_->test(ptr-ptr_))
	throw std::logic_error("Error!");
	ptr->~T();
	map_->reset(ptr - ptr_);
		
}


template<typename T>
auto allocator<T>::resize() -> void
{
	allocator<T> all(size_ * 2 + (size_ == 0));
	for (size_t i = 0; i < size_; ++i) 
		all.construct(all.get() + i, ptr_[i]);
	this->swap(all);
}


template<typename T>
auto allocator<T>::get() -> T* 
{ 
	return ptr_; 
}


template<typename T>
auto allocator<T>::get() const -> T const * 
{ 
	return ptr_; 
}


template<typename T>
allocator<T>::allocator(allocator const & other) : allocator<T>(other.size_)
{
	for (size_t i = 0; i < size_; ++i) 
		construct(ptr_ + i, other.ptr_[i]);
}


template <typename T>
auto allocator<T>::destroy(T *first,T *last) -> void
{
	if(first>last)
	throw std::logic_error("Error!");
	for (; first != last; ++first)
	{
		destroy(&*first);
	}
	
}


template<typename T>
auto allocator<T>::count() const -> size_t
{ 
	return map_->counter(); 
}


template<typename T>
auto allocator<T>::full() const -> bool 
{
	return (map_->counter() == size_); 
}


template<typename T>
auto allocator<T>::empty() const -> bool 
{ 
	return (map_->counter() == 0); 
}

	
template<typename T> 
auto allocator<T>::swap(allocator & other) -> void {
	std::swap(ptr_, other.ptr_);
	std::swap(size_, other.size_);
	std::swap(map_, other.map_);
}


//________________________________________________________________________________________________________________________________________
//________________________________________________________________________________________________________________________________________


template <typename T>
class stack
{
public:
	explicit
	stack(size_t size = 0);
	auto operator =(stack const & other) -> stack &;   /*strong*/
	auto empty() const -> bool;   /*noexcept*/
	auto count() const -> size_t;   /*noexcept*/
	auto push(T const & value) -> void;   /*strong*/
	auto pop() -> void;   /*strong*/
	auto top() -> T &;   /*strong*/
	auto top() const -> T const &;   /*strong*/
private:
	allocator<T> allocator_;
	auto throw_is_empty() const -> void;   /*strong*/
};


template <typename T>
stack<T>::stack(size_t size) : allocator_(size) {}


template <typename T>
auto stack<T>::operator=(const stack & st) -> stack & {
	if (this != &st)
	{
		(allocator<T>(st.allocator_)).swap(allocator_);
	}
	return *this;
}


template <typename T>
auto stack<T>::count() const -> size_t {
	return allocator_.count();
}


template <typename T>
auto stack<T>::push(T const & value) -> void {
	if (allocator_.full())
		allocator_.resize();
	allocator_.construct(allocator_.get() + this->count(), value);
}


template <typename T>
auto stack<T>::pop() -> void {
	if (allocator_.count() == 0) {
		this->throw_is_empty();
	}
	allocator_.destroy(allocator_.get() + (this->count()-1));
}


template <typename T>
auto stack<T>::top() -> T& {
	if (this->count() > 0) 
		return(*(allocator_.get() + this->count() - 1));
	else this->throw_is_empty();
}


template<typename T>
auto stack<T>::top() const -> T const & {
	if (this->count() > 0) 
		return(*(allocator_.get() + this->count() - 1));
	else this->throw_is_empty();
}


template <typename T>
auto stack<T>::empty() const -> bool {
	return(allocator_.count() == 0);
}


template<typename T>
auto stack<T>::throw_is_empty() const -> void {
	throw std::logic_error("Error!"); 
}
