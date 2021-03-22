/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: VectorDetails
 */

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
template<typename ...Args> requires std::constructible_from<Type, Args...>
inline Type &kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::push(Args &&...args)
    noexcept(nothrow_constructible(Type, Args...) && nothrow_forward_constructible(Type) && nothrow_destructible(Type))
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
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::insertDefault(Iterator pos, const Range count)
    noexcept(nothrow_default_constructible(Type) && nothrow_forward_constructible(Type) && nothrow_destructible(Type))
{
    if (!count) [[unlikely]]
        return end();
    else if (pos == nullptr) [[unlikely]] {
        resize(count);
        return beginUnsafe();
    }
    const auto currentData = dataUnsafe();
    const auto currentSize = sizeUnsafe();
    auto currentBegin = beginUnsafe();
    auto currentEnd = endUnsafe();
    Range position = pos - currentBegin;
    if (const auto currentCapacity = capacityUnsafe(), total = currentSize + count; total > currentCapacity) [[unlikely]] {
        const auto desiredCapacity = currentCapacity + std::max(currentCapacity, count);
        const auto tmpData = allocate(desiredCapacity);
        setData(tmpData);
        setSize(total);
        setCapacity(desiredCapacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData) {
                if (const auto after = currentSize - position; after > count) {
                    std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
                    std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
                } else
                    std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
                std::uninitialized_default_construct_n(tmpData + position, count);
                return tmpData + position;
            }
        }
        std::uninitialized_move_n(currentBegin, position, tmpData);
        std::uninitialized_move(currentBegin + position, currentEnd, tmpData + position + count);
        std::uninitialized_default_construct_n(tmpData + position, count);
        deallocate(currentData, currentCapacity);
        return tmpData + position;
    } else if (const auto after = currentSize - position; after > count) {
        std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
        std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
    } else
        std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
    std::uninitialized_default_construct_n(currentBegin + position, count);
    setSize(currentSize + count);
    return currentBegin + position;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::insertCopy(
        Iterator pos, const Range count, const Type &value)
    noexcept(nothrow_copy_constructible(Type) && nothrow_forward_constructible(Type) && nothrow_destructible(Type))
{
    if (!count) [[unlikely]]
        return end();
    else if (pos == nullptr) [[unlikely]] {
        resize(count, value);
        return beginUnsafe();
    }
    const auto currentData = dataUnsafe();
    const auto currentSize = sizeUnsafe();
    auto currentBegin = beginUnsafe();
    auto currentEnd = endUnsafe();
    Range position = pos - currentBegin;
    if (const auto currentCapacity = capacityUnsafe(), total = currentSize + count; total > currentCapacity) [[unlikely]] {
        const auto desiredCapacity = currentCapacity + std::max(currentCapacity, count);
        const auto tmpData = allocate(desiredCapacity);
        setData(tmpData);
        setSize(total);
        setCapacity(desiredCapacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData) {
                if (const auto after = currentSize - position; after > count) {
                    std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
                    std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
                } else
                    std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
                std::fill_n(tmpData + position, count, value);
                return tmpData + position;
            }
        }
        std::uninitialized_move_n(currentBegin, position, tmpData);
        std::uninitialized_move(currentBegin + position, currentEnd, tmpData + position + count);
        std::fill_n(tmpData + position, count, value);
        deallocate(currentData, currentCapacity);
        return tmpData + position;
    } else if (const auto after = currentSize - position; after > count) {
        std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
        std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
    } else
        std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
    std::fill_n(currentBegin + position, count, value);
    setSize(currentSize + count);
    return currentBegin + position;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
template<std::input_iterator InputIterator>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::insert(
        Iterator pos, InputIterator from, InputIterator to)
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
    const auto currentSize = sizeUnsafe();
    auto currentBegin = beginUnsafe();
    auto currentEnd = endUnsafe();
    if (const auto currentCapacity = capacityUnsafe(), total = currentSize + count; total > currentCapacity) [[unlikely]] {
        const auto desiredCapacity = currentCapacity + std::max(currentCapacity, count);
        const auto tmpData = allocate(desiredCapacity);
        setData(tmpData);
        setSize(total);
        setCapacity(desiredCapacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData) {
                if (const auto after = currentSize - position; after > count) {
                    std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
                    std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
                } else
                    std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
                std::copy(from, to, tmpData + position);
                return tmpData + position;
            }
        }
        std::uninitialized_move_n(currentBegin, position, tmpData);
        std::uninitialized_move(currentBegin + position, currentEnd, tmpData + position + count);
        std::copy(from, to, tmpData + position);
        deallocate(currentData, currentCapacity);
        return tmpData + position;
    } else if (const auto after = currentSize - position; after > count) {
        std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
        std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
    } else
        std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
    std::copy(from, to, currentBegin + position);
    setSize(currentSize + count);
    return currentBegin + position;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
template<std::input_iterator InputIterator, typename Map>
inline typename kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::Iterator
    kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::insert(
        Iterator pos, InputIterator from, InputIterator to, Map &&map)
{
    constexpr auto MapCopy = [](InputIterator from, InputIterator to, auto &&map, Iterator begin) {
        while (from != to) {
            if constexpr (Utils::IsMoveIterator<InputIterator>::Value)
                new (begin) Type(map(std::move(*from)));
            else
                new (begin) Type(map(*from));
            ++from;
            ++begin;
        }
    };

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
    const auto currentSize = sizeUnsafe();
    auto currentBegin = beginUnsafe();
    auto currentEnd = endUnsafe();
    if (const auto currentCapacity = capacityUnsafe(), total = currentSize + count; total > currentCapacity) [[unlikely]] {
        const auto desiredCapacity = currentCapacity + std::max(currentCapacity, count);
        const auto tmpData = allocate(desiredCapacity);
        setData(tmpData);
        setSize(total);
        setCapacity(desiredCapacity);
        if constexpr (IsSmallOptimized) {
            if (tmpData == currentData) {
                if (const auto after = currentSize - position; after > count) {
                    std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
                    std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
                } else
                    std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
                MapCopy(from, to, std::forward<Map>(map), tmpData + position);
                return tmpData + position;
            }
        }
        std::uninitialized_move_n(currentBegin, position, tmpData);
        std::uninitialized_move(currentBegin + position, currentEnd, tmpData + position + count);
        MapCopy(from, to, std::forward<Map>(map), tmpData + position);
        deallocate(currentData, currentCapacity);
        return tmpData + position;
    } else if (const auto after = currentSize - position; after > count) {
        std::uninitialized_move(currentEnd - count, currentEnd, currentEnd);
        std::move_backward(currentBegin + position, currentEnd - count, currentEnd);
    } else
        std::uninitialized_move(currentBegin + position, currentEnd, currentEnd + count - after);
    MapCopy(from, to, std::forward<Map>(map), currentBegin + position);
    setSize(currentSize + count);
    return currentBegin + position;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::erase(Iterator from, Iterator to)
    noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
{
    if (from == to) [[unlikely]]
        return;
    const auto end = endUnsafe();
    setSize(sizeUnsafe() - std::distance(from, to));
    std::destroy(from, to);
    std::uninitialized_move(to, end, from);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::resize(const Range count)
    noexcept(nothrow_default_constructible(Type) && nothrow_destructible(Type))
    requires std::constructible_from<Type>
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
    std::uninitialized_value_construct_n(dataUnsafe(), count);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::resize(const Range count, const Type &value)
    noexcept(nothrow_copy_constructible(Type) && nothrow_forward_constructible(Type) && nothrow_destructible(Type))
    requires std::copy_constructible<Type>
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
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::resize(InputIterator from, InputIterator to)
    noexcept(nothrow_forward_iterator_constructible(InputIterator) && nothrow_forward_constructible(Type) && nothrow_destructible(Type))
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
template<std::input_iterator InputIterator, typename Map>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::resize(InputIterator from, InputIterator to, Map &&map)
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
    auto begin = beginUnsafe();
    while (from != to) {
        if constexpr (Utils::IsMoveIterator<InputIterator>::Value)
            new (begin) Type(std::invoke(map, std::move(*from)));
        else
            new (begin) Type(std::invoke(map, *from));
        ++from;
        ++begin;
    }
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
    const auto currentCapacity = capacityUnsafe();

    clearUnsafe();
    setCapacity(0);
    setData(nullptr);
    deallocate(currentData, currentCapacity);
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
        const auto currentCapacity = capacityUnsafe();
        if (currentCapacity >= capacity) [[unlikely]]
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
        deallocate(currentData, currentCapacity);
        return true;
    } else {
        if (capacity == 0)
            return false;
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
    deallocate(currentData, currentCapacity);
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::move(Range from, Range to, Range output) noexcept_ndebug
{
    kFAssert(output < from || output > to,
        throw std::logic_error("VectorDetails::move: Invalid move range"));
    ++to;
    if (output < from) {
        const auto tmp = from;
        from = output;
        output = to;
        to = tmp;
    } else if (output)
        ++output;
    const auto it = beginUnsafe();
    std::rotate(it + from, it + to, it + output);
}
template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
inline bool kF::Core::Internal::VectorDetails<Base, Type, Range, IsSmallOptimized>::operator==(const VectorDetails &other) const noexcept
    requires std::equality_comparable<Type>
{
    const auto count = size();
    const auto otherCount = other.size();

    if (count == otherCount) [[unlikely]] {
        if (count) [[likely]]
            return std::equal(beginUnsafe(), endUnsafe(), other.beginUnsafe());
        else [[unlikely]]
            return true;
    } else [[likely]]
        return false;
}