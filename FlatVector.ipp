/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

#include <stdexcept>

template<typename Type>
inline Type *kF::Core::FlatVector<Type>::begin(void) noexcept
{
    if (_ptr) [[likely]]
        return data();
    else [[unlikely]]
        return nullptr;
}

template<typename Type>
inline const Type *kF::Core::FlatVector<Type>::begin(void) const noexcept
{
    if (_ptr) [[likely]]
        return data();
    else [[unlikely]]
        return nullptr;
}

template<typename Type>
inline Type *kF::Core::FlatVector<Type>::end(void) noexcept
{
    if (_ptr) [[likely]]
        return data() + _ptr->size;
    else [[unlikely]]
        return nullptr;
}

template<typename Type>
inline const Type *kF::Core::FlatVector<Type>::end(void) const noexcept
{
    if (_ptr) [[likely]]
        return data() + _ptr->size;
    else [[unlikely]]
        return nullptr;
}

template<typename Type>
inline std::size_t kF::Core::FlatVector<Type>::size(void) const noexcept
{
    if (_ptr) [[likely]]
        return _ptr->size;
    else [[unlikely]]
        return 0;
}

template<typename Type>
inline std::size_t kF::Core::FlatVector<Type>::capacity(void) const noexcept
{
    if (_ptr) [[likely]]
        return _ptr->capacity;
    else [[unlikely]]
        return 0;
}

template<typename Type>
template<typename ...Args>
inline void kF::Core::FlatVector<Type>::push(Args &&...args) noexcept(nothrow_ndebug && nothrow_constructible(Type, Args...))
{
    if (!_ptr) [[unlikely]]
        reserve(2);
    else if (_ptr->size == _ptr->capacity) [[unlikely]]
        grow();
    new (&at(_ptr->size)) Type(std::forward<Args>(args)...);
    ++_ptr->size;
}

template<typename Type>
inline void kF::Core::FlatVector<Type>::pop(void) noexcept_destructible(Type)
{
    --_ptr->size;
    at(_ptr->size).~Type();
}

template<typename Type>
inline void kF::Core::FlatVector<Type>::resize(const std::size_t count) noexcept(nothrow_ndebug && nothrow_constructible(Type) && nothrow_destructible(Type))
{
    if (!_ptr || _ptr->capacity < count) [[likely]]
        reserve(count);
    else [[unlikely]]
        clear();
    _ptr->size = count;
    for (auto &elem : *this)
        new (&elem) Type();
}

template<typename Type>
template<typename ...Args>
inline void kF::Core::FlatVector<Type>::resize(const std::size_t count, Args &&...args) noexcept(nothrow_ndebug && nothrow_constructible(Type, Args...) && nothrow_destructible(Type))
{
    if (!_ptr || _ptr->capacity < count) [[likely]]
        reserve(count);
    else [[unlikely]]
        clear();
    _ptr->size = count;
    for (auto &elem : *this)
        new (&elem) Type(std::forward<Args>(args)...);
}

template<typename Type>
inline void kF::Core::FlatVector<Type>::reserve(const std::size_t count) noexcept(nothrow_ndebug && nothrow_destructible(Type))
{
    if (_ptr) { [[unlikely]]
        if (_ptr->size != count) [[likely]]
            release();
        else [[unlikely]] {
            clear();
            return;
        }
    }
    _ptr = reinterpret_cast<Header *>(std::malloc(sizeof(Header) + sizeof(Type) * count));
    kFAssert(_ptr,
        throw std::runtime_error("Core::FlatVector: Malloc failed"));
    _ptr->size = 0;
    _ptr->capacity = count;
}

template<typename Type>
inline void kF::Core::FlatVector<Type>::clear(void) noexcept_destructible(Type)
{
    for (auto &elem : *this)
        elem.~Type();
}

template<typename Type>
inline void kF::Core::FlatVector<Type>::release(void) noexcept_destructible(Type)
{
    if (!_ptr) [[unlikely]]
        return;
    clear();
    std::free(_ptr);
    _ptr = nullptr;
}

template<typename Type>
void kF::Core::FlatVector<Type>::grow(void) noexcept_ndebug
{
    const auto size = _ptr->size;
    const auto capacity = _ptr->capacity * 2;
    auto ptr = reinterpret_cast<Header *>(std::malloc(sizeof(Header) + sizeof(Type) * capacity));
    auto data = reinterpret_cast<Type *>(ptr + 1);
    kFAssert(ptr,
        throw std::runtime_error("Core::FlatVector: Malloc failed"));
    ptr->size = size;
    ptr->capacity = capacity;
    for (auto i = 0ul; i < size; ++i)
        data[i] = std::move(at(i));
    std::free(_ptr);
    _ptr = ptr;
}