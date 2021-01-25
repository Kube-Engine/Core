/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sorted SmallVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "AllocatedSmallVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size, capacity and a small cache close to the data pointer
     * The vector guarantee that it will be sorted at any given time
     * The vector must take an allocator and a deallocator functor
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam AllocateFunc Allocator
     * @tparam DeallocateFunc Deallocator
     * @tparam Range Range of container
     * @tparam Compare Compare operator
     */
    template<typename Type, std::size_t OptimizedCapacity, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t, typename Compare = std::less<Type>>
    using SortedAllocatedSmallVector = Internal::SortedVectorDetails<Internal::AllocatedSmallVectorBase<Type, OptimizedCapacity, AllocateFunc, DeallocateFunc, Range>, Type, Range, Compare, true>;

    /** @brief Small optimized vector with a reduced range
     * The vector guarantee that it will be sorted at any given time
     * The vector must take an allocator and a deallocator functor */
    template<typename Type, std::size_t OptimizedCapacity, auto AllocateFunc, auto DeallocateFunc, typename Compare = std::less<Type>>
    using SortedAllocatedTinySmallVector = SortedAllocatedSmallVector<Type, OptimizedCapacity, AllocateFunc, DeallocateFunc, std::uint32_t, Compare>;
}