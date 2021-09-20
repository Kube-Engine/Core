/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVectorBase
 */

template<typename Type, std::integral Range, typename CustomHeaderType>
inline const Type *kF::Core::Internal::FlatVectorBase<Type, Range, CustomHeaderType>::data(void) const noexcept
{
    if (_ptr) [[likely]]
        return dataUnsafe();
    else
        return nullptr;
}

template<typename Type, std::integral Range, typename CustomHeaderType>
inline void kF::Core::Internal::FlatVectorBase<Type, Range, CustomHeaderType>::steal(FlatVectorBase &other) noexcept
{
    std::swap(_ptr, other._ptr);
}

template<typename Type, std::integral Range, typename CustomHeaderType>
inline Type *kF::Core::Internal::FlatVectorBase<Type, Range, CustomHeaderType>::allocate(const Range capacity) noexcept
{
    auto ptr = Utils::AlignedAlloc<alignof(Header), Header>(sizeof(Header) + sizeof(Type) * capacity);
    if constexpr (!std::is_same_v<CustomHeaderType, NoCustomHeaderType>) {
        if (_ptr)
            new (&ptr->customType) CustomHeaderType(std::move(_ptr->customType));
        else
            new (&ptr->customType) CustomHeaderType {};
    }
    return reinterpret_cast<Type *>(ptr + 1);
}

template<typename Type, std::integral Range, typename CustomHeaderType>
inline void kF::Core::Internal::FlatVectorBase<Type, Range, CustomHeaderType>::deallocate(Type * const data, const Range) noexcept
{
    auto ptr = reinterpret_cast<Header *>(data) - 1;
    if constexpr (!std::is_same_v<CustomHeaderType, NoCustomHeaderType>)
        ptr->customType.~CustomHeaderType();
    Utils::AlignedFree(ptr);
}