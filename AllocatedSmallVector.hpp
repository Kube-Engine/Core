/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedSmallVector
 */

#pragma once

#include "VectorDetails.hpp"
#include "AllocatedSmallVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size, capacity and a small cache close to the data pointer
     * The vector must take an allocator and a deallocator functor
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam AllocateFunc Allocator
     * @tparam DeallocateFunc Deallocator
     * @tparam Range Range of container
     */
    template<typename Type, std::size_t OptimizedCapacity, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t>
    using AllocatedSmallVector = Internal::VectorDetails<Internal::AllocatedSmallVectorBase<Type, OptimizedCapacity, AllocateFunc, DeallocateFunc, Range>, Type, Range, true>;

    /** @brief Small optimized vector with a reduced range
     * The vector must take an allocator and a deallocator functor */
    template<typename Type, std::size_t OptimizedCapacity, auto AllocateFunc, auto DeallocateFunc>
    using AllocatedTinySmallVector = AllocatedSmallVector<Type, OptimizedCapacity, AllocateFunc, DeallocateFunc, std::uint32_t>;
}
