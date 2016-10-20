#include <iostream>
#include <stdexcept>


template<typename T>
class allocator
{
protected:
	allocator(size_t size = 0);
	~allocator();
	auto swap(allocator& other)->void;
	allocator(allocator const&) = delete;
	auto operator=(allocator const&)->allocator& = delete;
	T * ptr_;
	size_t size_;
	size_t count_;
};

//__________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________


template <typename T1, typename T2>
auto construct(T1 * ptr, T2 const & value)->void {
	new(ptr) T1(value);
}

template <typename T>
void destroy(T * ptr) noexcept
{
	ptr->~T();
}

template <typename FwdIter>
void destroy(FwdIter first, FwdIter last) noexcept
{
	for (; first != last; ++first) {
		destroy(&*first);
	}
}

template <typename T>// êîíñòðóêòîð àëëîêàòîðà
allocator<T>::allocator(size_t size) : ptr_(static_cast<T *>(size == 0 ? nullptr : operator new(size * sizeof(T)))), size_(0), count_(size) {
};

template <typename T>//äåñòðóêòîð àëëîêàòîðà
allocator<T>::~allocator() {
operator delete(ptr_);
};

template <typename T>//swap allocator
auto allocator<T>::swap(allocator& other)->void {
	std::swap(ptr_, other.ptr_);
	std::swap(size_, other.size_);
	std::swap(count_, other.count_);
};

//__________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________
 
template <typename T>
class stack : private allocator<T>
{
public:
	stack();/*noexcept*/
	stack(stack const &); /*strong*/
	auto count() const noexcept->size_t;/*noexcept*/
	auto push(T const &)->void;/*strong*/
	auto pop()->void;/*strong*/
	auto top() const->T&;/*strong*/
	~stack(); 	/*noexcept*/
	auto operator=(const stack&tmp)->stack&;/*strong*/
	auto empty()->bool;/*noexcept*/

};

//__________________________________________________________________________________________________________________
//__________________________________________________________________________________________________________________



template<typename T> // ïðîâåðêà íà ïóñòîòó ñòýêà 
auto stack<T>::empty()->bool {
	return (allocator<T>::count_ == 0);
}

template <typename T>//êîïèðîâàíèå è âûäåëåíèå ïàìÿòè 
auto mem_copy(size_t count_m, size_t array_size_m, const T * tmp)->T* {
	T *mass = new T[array_size_m];
	std::copy(tmp, tmp + count_m, mass);
	return mass;
}


template <typename T>
stack<T>::~stack() {
destroy(allocator<T>::ptr_, allocator<T>::ptr_ + allocator<T>::count_);};

template <typename T>
stack<T>::stack()  {};



template <typename T>//âñòàâêà ýëåìåíòà â ñòýê 
auto stack<T>::push(T const &val)->void {
	if (allocator<T>::count_ == allocator<T>::size_) {//óâåëè÷èâàåò ïàìÿòü 
		size_t size = allocator<T>::size_ * 2 + (allocator<T>::size_ == 0);
		T *tmp = mem_copy(allocator<T>::count_, size, allocator<T>::ptr_);
		delete[] allocator<T>::ptr_;
		allocator<T>::ptr_ = tmp;
		allocator<T>::size_ = size;
	}
	construct(allocator<T>::ptr_+allocator<T>::count_,val);
	++allocator<T>::count_;
}


template <typename T>//êîíñòðóêòîð êîïèðîâàíèÿ
stack<T>::stack(stack const &tmp) {
	allocator<T>::count_ = tmp.count_;
	allocator<T>::ptr_ = mem_copy(tmp.count_, tmp.size_, tmp.ptr_);
	allocator<T>::size_ = tmp.size_;
};

template <typename T>//ïåðåãðóçêà îïåðàòîðà ïðèñâàèâàíèÿ 
auto stack<T>::operator=(const stack &tmp)->stack& {
	if (this != &tmp) {
		T* cp = mem_copy(tmp.count_, tmp.size_, tmp.ptr_);
		delete[] allocator<T>::ptr_; 
		allocator<T>::ptr_ = cp;
		allocator<T>::size_ =tmp.size_;
		allocator<T>::count_ = tmp.count_;
	}
	return *this;
}


template <typename T>//âîçâðàùàåì count_
auto stack<T>::count() const noexcept->size_t {
	return allocator<T>::count_;
}

template <typename T>// óìåíüøåíèå count_ 
auto stack<T>::pop()->void {
	if (allocator<T>::count_ == 0) throw std::logic_error("Empty!");
	destroy(allocator<T>::ptr_+allocator<T>::count_);
	--allocator<T>::count_;
}

template <typename T>//óäàëåíèå ýëåìåíòà
auto stack<T>::top() const->T& {
	if (allocator<T>::count_ == 0) throw std::logic_error("Empty!");
	return allocator<T>::ptr_[allocator<T>::count_-1];

}
