/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

template<typename Type, std::integral Range>
inline const Type *kF::Core::Internal::FlatVectorBase<Type, Range>::data(void) const noexcept
{
    if (_ptr) [[likely]]
        return dataUnsafe();
    else
        return nullptr;
}

template<typename Type, std::integral Range>
inline void kF::Core::Internal::FlatVectorBase<Type, Range>::steal(FlatVectorBase &other) noexcept
{
    std::swap(_ptr, other._ptr);
}