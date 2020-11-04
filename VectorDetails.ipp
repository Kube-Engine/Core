/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: VectorDetails
 */

template<typename Base, typename Type, std::integral Range>
template<typename ...Args>
inline Type &kF::Core::Internal::VectorDetails<Base, Type, Range>::push(Args &&...args)
    noexcept(std::is_nothrow_constructible_v<Type, Args...> && nothrow_destructible(Type))
    requires std::constructible_from<Type, Args...>
{
    if (!data()) [[unlikely]]
        reserve(2);
    else if (sizeUnsafe() == capacityUnsafe()) [[unlikely]]
        grow();
    const auto currentSize = sizeUnsafe();
    Type * const elem = data() + currentSize;
    setSize(currentSize + 1);
    new (elem) Type(std::forward<Args>(args)...);
    return *elem;
}

template<typename Base, typename Type, std::integral Range>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range>::pop(void) noexcept_destructible(Type)
{
    const auto desiredSize = sizeUnsafe() - 1;

    data()[desiredSize].~Type();
    setSize(desiredSize);
}

template<typename Base, typename Type, std::integral Range>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range>::reserve(const Range capacity) noexcept_destructible(Type)
{
    if (data()) [[unlikely]] {
        if (sizeUnsafe() != capacity) [[likely]]
            releaseUnsafe();
        else [[unlikely]]
            return clearUnsafe();
    }
    setData(allocate(capacity));
    setSize(0);
    setCapacity(capacity);
}

template<typename Base, typename Type, std::integral Range>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range>::clear(void) noexcept_destructible(Type)
{
    if (data()) [[likely]]
        clearUnsafe();
}

template<typename Base, typename Type, std::integral Range>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range>::clearUnsafe(void) noexcept_destructible(Type)
{
    std::destroy_n(data(), sizeUnsafe());
}

template<typename Base, typename Type, std::integral Range>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range>::release(void) noexcept_destructible(Type)
{
    if (data()) [[likely]]
        releaseUnsafe();
}

template<typename Base, typename Type, std::integral Range>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range>::releaseUnsafe(void) noexcept_destructible(Type)
{
    clearUnsafe();
    deallocate(data());
}

template<typename Base, typename Type, std::integral Range>
inline void kF::Core::Internal::VectorDetails<Base, Type, Range>::grow(const Range minimum) noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
{
    const auto currentData = data();
    const auto currentSize = sizeUnsafe();
    const auto currentCapacity = capacityUnsafe();
    const auto desiredCapacity = currentCapacity + std::max(currentCapacity, minimum);
    const auto tmpData = allocate(desiredCapacity);

    std::uninitialized_move_n(currentData, currentSize, tmpData);
    std::destroy_n(currentData, currentSize);
    deallocate(currentData);
    setData(tmpData);
    setSize(currentSize);
    setCapacity(desiredCapacity);
}
