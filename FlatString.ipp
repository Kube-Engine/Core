/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatString
 */

template<typename Type>
template<bool SafeCheck>
inline kF::Core::FlatString<Type>::Iterator kF::Core::FlatString<Type>::begin(void) noexcept
{
    if constexpr (SafeCheck) {
        if (_ptr) [[likely]]
            return data();
        else [[unlikely]]
            return nullptr;
    } else
        return data();
}

template<typename Type>
template<bool SafeCheck>
inline kF::Core::FlatString<Type>::ConstIterator kF::Core::FlatString<Type>::begin(void) const noexcept
{
    if constexpr (SafeCheck) {
        if (_ptr) [[likely]]
            return data();
        else [[unlikely]]
            return nullptr;
    } else
        return data();
}

template<typename Type>
template<bool SafeCheck>
inline kF::Core::FlatString<Type>::Iterator kF::Core::FlatString<Type>::end(void) noexcept
{
    if constexpr (SafeCheck) {
        if (_ptr) [[likely]]
            return data() + _ptr->size;
        else [[unlikely]]
            return nullptr;
    } else
        return data() + _ptr->size;
}

template<typename Type>
template<bool SafeCheck>
inline kF::Core::FlatString<Type>::ConstIterator kF::Core::FlatString<Type>::end(void) const noexcept
{
    if constexpr (SafeCheck) {
        if (_ptr) [[likely]]
            return data() + _ptr->size;
        else [[unlikely]]
            return nullptr;
    } else
        return data() + _ptr->size;
}

template<typename Type>
template<bool SafeCheck>
inline std::size_t kF::Core::FlatString<Type>::size(void) const noexcept
{
    if constexpr (SafeCheck) {
        if (_ptr) [[likely]]
            return _ptr->size;
        else [[unlikely]]
            return 0;
    } else
        return _ptr->size;
}

template<typename Type>
template<bool SafeCheck>
inline std::size_t kF::Core::FlatString<Type>::capacity(void) const noexcept
{
    if constexpr (SafeCheck) {
        if (_ptr) [[likely]]
            return _ptr->capacity;
        else [[unlikely]]
            return 0;
    } else
        return _ptr->capacity;
}

template<typename Type>
template<typename ...Args>
inline void kF::Core::FlatString<Type>::push(Args &&...args)
    noexcept(nothrow_constructible(Type, Args...) && nothrow_destructible(Type))
    requires std::constructible_from<Type, Args...>
{
    if (!_ptr) [[unlikely]]
        reserve(2);
    else if (_ptr->size == _ptr->capacity) [[unlikely]]
        grow();
    new (&at(_ptr->size)) Type(std::forward<Args>(args)...);
    ++_ptr->size;
}

template<typename Type>
inline void kF::Core::FlatString<Type>::pop(void) noexcept_destructible(Type)
{
    --_ptr->size;
    at(_ptr->size).~Type();
}

template<typename Type>
inline void kF::Core::FlatString<Type>::resize(const std::size_t count)
    noexcept(nothrow_constructible(Type) && nothrow_destructible(Type))
    requires std::constructible_from<Type>
{
    if (!_ptr || _ptr->capacity < count) [[likely]]
        reserve(count);
    else [[unlikely]]
        clear();
    _ptr->size = count;
    std::uninitialized_default_construct_n(data(), count);
}

template<typename Type>
inline void kF::Core::FlatString<Type>::resize(const std::size_t count, const Type &value)
    noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type))
    requires std::copy_constructible<Type>
{
    if (!_ptr || _ptr->capacity < count) [[likely]]
        reserve(count);
    else [[unlikely]]
        clear();
    _ptr->size = count;
    std::uninitialized_fill_n(data(), count, value);
}

template<typename Type>
template<std::input_iterator InputIterator>
inline void kF::Core::FlatString<Type>::resize(const InputIterator from, const InputIterator to)
    noexcept(nothrow_destructible(Type) && nothrow_forward_iterator_constructible(InputIterator))
{
    const auto count = to - from;

    reserve(count);
    std::uninitialized_copy(from, to, begin());
    _ptr->size = count;
}

template<typename Type>
template<std::input_iterator InputIterator>
kF::Core::FlatString<Type>::Iterator kF::Core::FlatString<Type>::insert(const Iterator at, const InputIterator from, const InputIterator to)
    noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
{
    const auto count { static_cast<std::size_t>(std::distance(from, to)) };
    std::size_t position;
    if (at == nullptr) [[unlikely]] {
        reserve(count);
        position = 0;
    } else
        position = at - begin<false>();
    if (const auto currentSize = size<false>(), total = currentSize + count; total > capacity<false>()) [[unlikely]] {
        const auto tmpSize = currentSize + std::max(currentSize, count);
        const auto tmpPtr = reinterpret_cast<Header *>(std::malloc(sizeof(Header) + sizeof(Type) * tmpSize));
        const auto tmpData = reinterpret_cast<Type *>(tmpPtr + 1);
        tmpPtr->size = total;
        tmpPtr->capacity = tmpSize;
        std::uninitialized_move_n(data(), position, tmpData);
        std::uninitialized_move_n(data() + position, count, tmpData + position + count);
        std::copy(from, to, tmpData + position);
        std::free(_ptr);
        _ptr = tmpPtr;
        return tmpData + position;
    }
    const auto tmpBegin = begin<false>();
    const auto tmpEnd = end<false>();
    if (const auto after = size<false>() - position; after > count) {
        std::uninitialized_move(tmpEnd - count, tmpEnd, tmpEnd);
        std::copy(from, to, tmpBegin + position);
    } else {
        auto mid = from;
        std::advance(mid, after);
        std::uninitialized_move(mid, to, tmpEnd);
        std::uninitialized_move(tmpBegin + position, tmpEnd, tmpEnd + count - after);
        std::copy(from, to, tmpBegin + position);
    }
    _ptr->size += count;
    return tmpBegin + position;
}

template<typename Type>
kF::Core::FlatString<Type>::Iterator kF::Core::FlatString<Type>::insert(const Iterator at, const std::size_t count, const Type &value)
    noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type))
    requires std::copy_constructible<Type>
{
    if (at == nullptr) [[unlikely]] {
        resize(count, value);
        return begin();
    }
    std::size_t position = at - begin<false>();
    if (const auto currentSize = size<false>(), total = currentSize + count; total > capacity<false>()) [[unlikely]] {
        const auto tmpSize = currentSize + std::max(currentSize, count);
        const auto tmpPtr = reinterpret_cast<Header *>(std::malloc(sizeof(Header) + sizeof(Type) * tmpSize));
        const auto tmpData = reinterpret_cast<Type *>(tmpPtr + 1);
        tmpPtr->size = total;
        tmpPtr->capacity = tmpSize;
        std::uninitialized_move_n(data(), position, tmpData);
        std::uninitialized_move_n(data() + position, count, tmpData + position + count);
        std::fill_n(tmpData + position, count, value);
        std::free(_ptr);
        _ptr = tmpPtr;
        return tmpData + position;
    }
    const auto tmpBegin = begin<false>();
    const auto tmpEnd = end<false>();
    if (const auto after = size<false>() - position; after > count) {
        std::uninitialized_move(tmpEnd - count, tmpEnd, tmpEnd);
        std::fill_n(tmpBegin + position, count, value);
    } else {
        const auto mid = count - after;
        std::fill_n(tmpEnd, mid, value);
        std::uninitialized_move(tmpBegin + position, tmpEnd, tmpEnd + mid);
        std::fill_n(tmpBegin + position, count - mid, value);
    }
    _ptr->size += count;
    return tmpBegin + position;
}

template<typename Type>
inline void kF::Core::FlatString<Type>::reserve(const std::size_t count) noexcept_destructible(Type)
{
    if (_ptr) { [[unlikely]]
        if (size<false>() != count) [[likely]]
            release<false>();
        else [[unlikely]] {
            clear<false>();
            return;
        }
    }
    _ptr = reinterpret_cast<Header *>(std::malloc(sizeof(Header) + sizeof(Type) * count));
    _ptr->size = 0;
    _ptr->capacity = count;
}

template<typename Type>
template<bool SafeCheck>
inline void kF::Core::FlatString<Type>::clear(void) noexcept_destructible(Type)
{
    if constexpr (SafeCheck) {
        if (!_ptr) [[unlikely]]
            return;
    }
    std::destroy_n(begin<false>(), size<false>());
}

template<typename Type>
template<bool SafeCheck>
inline void kF::Core::FlatString<Type>::release(void) noexcept_destructible(Type)
{
    if constexpr (SafeCheck) {
        if (!_ptr) [[unlikely]]
            return;
    }
    clear<false>();
    std::free(_ptr);
    _ptr = nullptr;
}

template<typename Type>
void kF::Core::FlatString<Type>::grow(const std::size_t minimum)
    noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
{
    const auto tmpSize = _ptr->size;
    const auto tmpCapacity = _ptr->capacity + std::max(_ptr->capacity, minimum);
    const auto tmpPtr = reinterpret_cast<Header *>(std::malloc(sizeof(Header) + sizeof(Type) * tmpCapacity));
    const auto tmpData = reinterpret_cast<Type *>(tmpPtr + 1);
    tmpPtr->size = tmpSize;
    tmpPtr->capacity = tmpCapacity;
    std::uninitialized_move_n(data(), tmpSize, tmpData);
    std::destroy_n(begin(), tmpSize);
    std::free(_ptr);
    _ptr = tmpPtr;
}