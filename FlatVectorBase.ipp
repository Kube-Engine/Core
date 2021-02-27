/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
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