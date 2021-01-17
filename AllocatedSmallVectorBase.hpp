/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Small Vector
 */

#pragma once

#include "SmallVectorBase.hpp"

namespace kF::Core::Internal
{
    template<typename Type, std::size_t OptimizedCapacity, auto AllocateFunc, auto DeallocateFunc, std::integral Range>
    class AllocatedSmallVectorBase;
}

/** @brief Base implementation of a vector with size and capacity cached */
template<typename Type, std::size_t OptimizedCapacity, auto AllocateFunc, auto DeallocateFunc, std::integral Range>
class kF::Core::Internal::AllocatedSmallVectorBase : public SmallVectorBase<Type, OptimizedCapacity, Range>
{
protected:
    /** @brief Allocates a new buffer */
    [[nodiscard]] Type *allocate(const Range capacity) noexcept
    {
        if (capacity <= OptimizedCapacity) [[likely]]
            return SmallVectorBase<Type, OptimizedCapacity, Range>::optimizedData();
        else
            return reinterpret_cast<Type *>(AllocateFunc(sizeof(Type) * capacity, alignof(Type)));
    }

    /** @brief Deallocates a buffer */
    void deallocate(Type * const data, const Range capacity) noexcept
    {
        if (data != SmallVectorBase<Type, OptimizedCapacity, Range>::optimizedData()) [[unlikely]]
            DeallocateFunc(data, capacity, alignof(Type));
    }
};
