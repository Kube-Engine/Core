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
    if (_ptr) {
        std::destroy(beginUnsafe(), endUnsafe());
        deallocate(dataUnsafe());
    }
    _ptr = other._ptr;
    other._ptr = nullptr;
}