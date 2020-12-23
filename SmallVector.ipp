/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Small Vector
 */

template<typename Type, std::size_t OptimizedCapacity, std::integral Range>
Type *kF::Core::Internal::SmallVectorBase<Type, OptimizedCapacity, Range>::allocate(const Range capacity) noexcept
{
    if (capacity <= OptimizedCapacity) [[likely]]
        return optimizedData();
    else
        return reinterpret_cast<Type *>(Utils::AlignedAlloc<alignof(Type)>(sizeof(Type) * capacity));
}

template<typename Type, std::size_t OptimizedCapacity, std::integral Range>
void kF::Core::Internal::SmallVectorBase<Type, OptimizedCapacity, Range>::deallocate(Type *data) noexcept
{
    if (data != optimizedData()) [[unlikely]]
        std::free(data);
}
