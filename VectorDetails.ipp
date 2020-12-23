/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: VectorDetails
 */

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
template<typename ...Args>
inline Type &kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::push(Args &&...args)
    noexcept(std::is_nothrow_constructible_v<Type, Args...> && nothrow_destructible(Type))
    requires std::constructible_from<Type, Args...>
{
    if (!data()) [[unlikely]]
        reserveUnsafe<false>(2);
    else if (sizeUnsafe() == capacityUnsafe()) [[unlikely]]
        grow();
    const auto currentSize = sizeUnsafe();
    Type * const elem = dataUnsafe() + currentSize;
    setSize(currentSize + 1);
    new (elem) Type(std::forward<Args>(args)...);
    return *elem;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::pop(void) noexcept_destructible(Type)
{
    const auto desiredSize = sizeUnsafe() - 1;

    dataUnsafe()[desiredSize].~Type();
    setSize(desiredSize);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::insert(const Iterator pos, const Range count)
    noexcept(nothrow_constructible(Type) && nothrow_destructible(Type))
{
    if (!count) [[unlikely]]
        return end();
    else if (pos == nullptr) [[unlikely]] {
        resize(count);
        return beginUnsafe();
    }
    const auto currentData = dataUnsafe();
    const auto currentBegin = beginUnsafe();
    const auto currentEnd = endUnsafe();
    const auto currentSize = sizeUnsafe();
    Range position = pos - currentBegin;
    if (const auto currentCapacity = capacityUnsafe(), total = currentSize + count; total > currentCapacity) [[unlikely]] {
        const auto desiredCapacity = currentCapacity + std::max(currentCapacity, count);
        const auto tmpData = allocate(desiredCapacity);
        setData(tmpData);
        setSize(total);
        setCapacity(desiredCapacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData) {
                std::uninitialized_move(currentBegin + position, currentEnd, tmpData + position + count);
                std::uninitialized_default_construct_n(tmpData + position, count);
                return tmpData + position;
            }
        }
        std::uninitialized_move_n(currentBegin, position, tmpData);
        std::uninitialized_move(currentBegin + position, currentEnd, tmpData + position + count);
        std::uninitialized_default_construct_n(tmpData + position, count);
        deallocate(currentData);
        return tmpData + position;
    } else if (const auto after = sizeUnsafe() - position; after > count) {
        std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
        std::uninitialized_default_construct_n(currentBegin + position, count);
    } else {
        const auto mid = count - after;
        std::uninitialized_default_construct_n(currentEnd, mid);
        std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + mid);
        std::uninitialized_default_construct_n(currentBegin + position, count - mid);
    }
    setSize(currentSize + count);
    return currentBegin + position;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::insert(const Iterator pos, const Range count, const Type &value)
    noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type))
{
    if (!count) [[unlikely]]
        return end();
    else if (pos == nullptr) [[unlikely]] {
        resize(count, value);
        return beginUnsafe();
    }
    const auto currentData = dataUnsafe();
    const auto currentBegin = beginUnsafe();
    const auto currentEnd = endUnsafe();
    const auto currentSize = sizeUnsafe();
    Range position = pos - currentBegin;
    if (const auto currentCapacity = capacityUnsafe(), total = currentSize + count; total > currentCapacity) [[unlikely]] {
        const auto desiredCapacity = currentCapacity + std::max(currentCapacity, count);
        const auto tmpData = allocate(desiredCapacity);
        setData(tmpData);
        setSize(total);
        setCapacity(desiredCapacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData) {
                std::uninitialized_move(currentBegin + position, currentEnd, tmpData + position + count);
                std::fill_n(tmpData + position, count, value);
                return tmpData + position;
            }
        }
        std::uninitialized_move_n(currentBegin, position, tmpData);
        std::uninitialized_move(currentBegin + position, currentEnd, tmpData + position + count);
        std::fill_n(tmpData + position, count, value);
        deallocate(currentData);
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
    setSize(currentSize + count);
    return currentBegin + position;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
template<std::input_iterator InputIterator> requires std::constructible_from<Type, decltype(*std::declval<InputIterator>())>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::insert(const Iterator pos, const InputIterator from, const InputIterator to)
    noexcept(nothrow_forward_iterator_constructible(InputIterator) && nothrow_forward_constructible(Type) && nothrow_destructible(Type))
{
    const Range count = std::distance(from, to);
    Range position;

    if (!count) [[unlikely]]
        return end();
    else if (pos == Iterator()) [[unlikely]] {
        reserve(count);
        position = 0;
    } else [[likely]]
        position = pos - beginUnsafe();
    const auto currentData = dataUnsafe();
    const auto currentBegin = beginUnsafe();
    const auto currentSize = sizeUnsafe();
    if (const auto currentCapacity = capacityUnsafe(), total = currentSize + count; total > currentCapacity) [[unlikely]] {
        const auto desiredCapacity = currentCapacity + std::max(currentCapacity, count);
        const auto tmpData = allocate(desiredCapacity);
        setData(tmpData);
        setSize(total);
        setCapacity(desiredCapacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData) {
                std::uninitialized_move_n(currentBegin + position, count, tmpData + position + count);
                std::copy(from, to, tmpData + position);
                return tmpData + position;
            }
        }
        std::uninitialized_move_n(currentBegin, position, tmpData);
        std::uninitialized_move_n(currentBegin + position, count, tmpData + position + count);
        std::copy(from, to, tmpData + position);
        deallocate(currentData);
        return tmpData + position;
    }
    const auto currentEnd = endUnsafe();
    if (const auto after = currentSize - position; after > count) {
        std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
        std::copy(from, to, currentBegin + position);
    } else {
        auto mid = from;
        std::advance(mid, after);
        std::uninitialized_move(mid, to, currentEnd);
        std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
        std::copy(from, to, currentBegin + position);
    }
    setSize(currentSize + count);
    return currentBegin + position;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::erase(const Iterator from, const Iterator to)
    noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
{
    if (from == to) [[unlikely]]
        return;
    const auto end = endUnsafe();
    setSize(sizeUnsafe() - std::distance(from, to));
    std::copy(std::make_move_iterator(to), std::make_move_iterator(end), from);
    std::destroy(to, end);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::resize(const Range count)
    noexcept(std::is_nothrow_constructible_v<Type> && nothrow_destructible(Type))
{
    if (!count) [[unlikely]] {
        clear();
        return;
    } else if (!data()) [[likely]]
        reserveUnsafe<false>(count);
    else if (capacityUnsafe() < count)
        reserveUnsafe<true>(count);
    else [[unlikely]]
        clearUnsafe();
    setSize(count);
    std::uninitialized_default_construct_n(dataUnsafe(), count);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::resize(const Range count, const Type &value)
    noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type))
{
    if (!count) [[unlikely]] {
        clear();
        return;
    } else if (!data()) [[likely]]
        reserveUnsafe<false>(count);
    else if (capacityUnsafe() < count)
        reserveUnsafe<true>(count);
    else [[unlikely]]
        clearUnsafe();
    setSize(count);
    std::uninitialized_fill_n(dataUnsafe(), count, value);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
template<std::input_iterator InputIterator>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::resize(const InputIterator from, const InputIterator to)
    noexcept(nothrow_destructible(Type) && nothrow_forward_iterator_constructible(InputIterator))
{
    const Range count = std::distance(from, to);

    if (!count) [[unlikely]] {
        clear();
        return;
    } else if (!data()) [[likely]]
        reserveUnsafe<false>(count);
    else if (capacityUnsafe() < count)
        reserveUnsafe<true>(count);
    else [[unlikely]]
        clearUnsafe();
    setSize(count);
    std::uninitialized_copy(from, to, beginUnsafe());
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::clear(void) noexcept_destructible(Type)
{
    if (data()) [[likely]]
        clearUnsafe();
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::clearUnsafe(void) noexcept_destructible(Type)
{
    std::destroy_n(dataUnsafe(), sizeUnsafe());
    setSize(0);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::release(void) noexcept_destructible(Type)
{
    if (data()) [[likely]]
        releaseUnsafe();
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::releaseUnsafe(void) noexcept_destructible(Type)
{
    const auto currentData = dataUnsafe();

    clearUnsafe();
    setCapacity(0);
    setData(nullptr);
    deallocate(currentData);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline bool kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::reserve(const Range capacity)
    noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
{
    if (data())
        return reserveUnsafe<true>(capacity);
    else
        return reserveUnsafe<false>(capacity);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
template<bool IsSafe>
inline bool kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::reserveUnsafe(const Range capacity)
    noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
{
    if constexpr (IsSafe) {
        if (capacityUnsafe() >= capacity) [[unlikely]]
            return false;
        const auto currentSize = sizeUnsafe();
        const auto currentData = dataUnsafe();
        const auto tmpData = allocate(capacity);
        setData(tmpData);
        setSize(currentSize);
        setCapacity(capacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData)
                return false;
        }
        std::uninitialized_move_n(currentData, currentSize, tmpData);
        std::destroy_n(currentData, currentSize);
        deallocate(currentData);
        return true;
    } else {
        setData(allocate(capacity));
        setSize(0);
        setCapacity(capacity);
        return true;
    }
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::grow(const Range minimum)
    noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
{
    const auto currentData = dataUnsafe();
    const auto currentSize = sizeUnsafe();
    const auto currentCapacity = capacityUnsafe();
    const auto desiredCapacity = currentCapacity + std::max(currentCapacity, minimum);
    const auto tmpData = allocate(desiredCapacity);

    setData(tmpData);
    setSize(currentSize);
    setCapacity(desiredCapacity);
    if constexpr (IsSmallOptimized) {
        if (tmpData == currentData)
            return;
    }
    std::uninitialized_move_n(currentData, currentSize, tmpData);
    std::destroy_n(currentData, currentSize);
    deallocate(currentData);
}
