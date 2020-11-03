/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

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
inline typename kF::Core::FlatVector<Type>::ConstIterator kF::Core::FlatVector<Type>::begin(void) const noexcept
{
    if (_ptr) [[likely]]
        return data();
    else [[unlikely]]
        return nullptr;
}

template<typename Type>
inline typename kF::Core::FlatVector<Type>::ConstIterator kF::Core::FlatVector<Type>::end(void) const noexcept
{
    if (_ptr) [[likely]]
        return data() + _ptr->size;
    else [[unlikely]]
        return nullptr;
}

template<typename Type>
template<typename ...Args>
inline Type &kF::Core::FlatVector<Type>::push(Args &&...args)
    noexcept(std::is_nothrow_constructible_v<Type, Args...> && nothrow_destructible(Type))
    requires std::constructible_from<Type, Args...>
{
    if (!_ptr) [[unlikely]]
        reserve(2);
    else if (_ptr->size == _ptr->capacity) [[unlikely]]
        grow();
    Type *data = &at(_ptr->size);
    ++_ptr->size;
    new (data) Type(std::forward<Args>(args)...);
    return *data;
}

template<typename Type>
inline void kF::Core::FlatVector<Type>::pop(void) noexcept_destructible(Type)
{
    --_ptr->size;
    at(_ptr->size).~Type();
}

template<typename Type>
inline void kF::Core::FlatVector<Type>::erase(const Iterator from, const Iterator to)
    noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
{
    if (from == to) [[unlikely]]
        return;
    const auto end = endUnsafe();
    if constexpr (std::is_move_assignable_v<Type> && !Utils::IsMoveIterator<Iterator>::Value)
        std::copy(std::make_move_iterator(to), std::make_move_iterator(end), from);
    else
        std::copy(to, end, from);
    std::destroy(to, end);
    _ptr->size -= std::distance(from, to);
}

template<typename Type>
inline void kF::Core::FlatVector<Type>::resize(const std::size_t count)
    noexcept(std::is_nothrow_constructible_v<Type> && nothrow_destructible(Type))
{
    if (!count) [[unlikely]] {
        clear();
        return;
    } else if (!_ptr || _ptr->capacity < count) [[likely]]
        reserve(count);
    else [[unlikely]]
        clear();
    _ptr->size = count;
    std::uninitialized_default_construct_n(data(), count);
}

template<typename Type>
inline void kF::Core::FlatVector<Type>::resize(const std::size_t count, const Type &value)
    noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type))
{
    if (!count) [[unlikely]] {
        clear();
        return;
    } else if (!_ptr || _ptr->capacity < count) [[likely]]
        reserve(count);
    else [[unlikely]]
        clear();
    _ptr->size = count;
    std::uninitialized_fill_n(data(), count, value);
}

template<typename Type>
template<std::input_iterator InputIterator>
inline void kF::Core::FlatVector<Type>::resize(const InputIterator from, const InputIterator to)
    noexcept(nothrow_destructible(Type) && nothrow_forward_iterator_constructible(InputIterator))
{
    const std::size_t count = std::distance(from, to);

    if (!count) [[unlikely]] {
        clear();
        return;
    }
    reserve(count);
    std::uninitialized_copy(from, to, begin());
    _ptr->size = count;
}

template<typename Type>
template<std::input_iterator InputIterator> requires std::constructible_from<Type, decltype(*std::declval<InputIterator>())>
inline typename kF::Core::FlatVector<Type>::Iterator kF::Core::FlatVector<Type>::insert(const Iterator pos, const InputIterator from, const InputIterator to)
    noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type)){
    const std::size_t count = std::distance(from, to);
    std::size_t position;

    if (!count) [[unlikely]]
        return end();
    else if (pos == nullptr) [[unlikely]] {
        reserve(count);
        position = 0;
    } else [[likely]]
        position = pos - beginUnsafe();
    if (const auto currentSize = sizeUnsafe(), total = currentSize + count; total > capacityUnsafe()) [[unlikely]] {
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
    const auto tmpBegin = beginUnsafe();
    const auto tmpEnd = endUnsafe();
    if (const auto after = sizeUnsafe() - position; after > count) {
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
inline typename kF::Core::FlatVector<Type>::Iterator kF::Core::FlatVector<Type>::insert(const Iterator pos, const std::size_t count, const Type &value)
    noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type))
{
    if (!count) [[unlikely]]
        return end();
    else if (pos == nullptr) [[unlikely]] {
        resize(count, value);
        return begin();
    }
    std::size_t position = pos - beginUnsafe();
    const auto currentBegin = beginUnsafe();
    const auto currentEnd = endUnsafe();
    const auto currentSize = sizeUnsafe();
    if (auto total = currentSize + count; total > capacityUnsafe()) [[unlikely]] {
        const auto tmpSize = currentSize + std::max(currentSize, count);
        const auto tmpPtr = reinterpret_cast<Header *>(std::malloc(sizeof(Header) + sizeof(Type) * tmpSize));
        const auto tmpData = reinterpret_cast<Type *>(tmpPtr + 1);
        tmpPtr->size = total;
        tmpPtr->capacity = tmpSize;
        std::uninitialized_move_n(currentBegin, position, tmpData);
        std::uninitialized_move(currentBegin + position, currentEnd, tmpData + position + count);
        std::fill_n(tmpData + position, count, value);
        std::free(_ptr);
        _ptr = tmpPtr;
        return tmpData + position;
    } else if (const auto after = sizeUnsafe() - position; after > count) {
        std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
        std::fill_n(currentBegin + position, count, value);
    } else {
        const auto mid = count - after;
        std::fill_n(currentEnd, mid, value);
        std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + mid);
        std::fill_n(currentBegin + position, count - mid, value);
    }
    _ptr->size += count;
    return currentBegin + position;
}

template<typename Type>
inline void kF::Core::FlatVector<Type>::reserve(const std::size_t count) noexcept_destructible(Type)
{
    if (_ptr) { [[unlikely]]
        if (sizeUnsafe() != count) [[likely]]
            release<false>();
        else [[unlikely]] {
            clear<false>();
            return;
        }
    }
    auto total = sizeof(Header) + sizeof(Type) * count;
    _ptr = reinterpret_cast<Header *>(std::malloc(total));
    _ptr->size = 0;
    _ptr->capacity = count;
}

template<typename Type>
template<bool SafeCheck>
inline void kF::Core::FlatVector<Type>::clear(void) noexcept_destructible(Type)
{
    if constexpr (SafeCheck) {
        if (!_ptr) [[unlikely]]
            return;
    }
    std::destroy_n(beginUnsafe(), sizeUnsafe());
    _ptr->size = 0;
}

template<typename Type>
template<bool SafeCheck>
inline void kF::Core::FlatVector<Type>::release(void) noexcept_destructible(Type)
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
void kF::Core::FlatVector<Type>::grow(const std::size_t minimum)
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