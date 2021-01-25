/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sorted Vector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "AllocatedVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size and capacity close to the data pointer
     * With default range (std::size_t), the vector takes 24 bytes
     * The vector guarantee that it will be sorted at any given time
     * The vector must take an allocator and a deallocator functor
     *
     * @tparam Type Internal type in container
     * @tparam AllocateFunc Allocator
     * @tparam DeallocateFunc Deallocator
     * @tparam Range Range of container
     * @tparam Compare Compare operator
     */
    template<typename Type, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t, typename Compare = std::less<Type>>
    using SortedAllocatedVector = Internal::SortedVectorDetails<Internal::AllocatedVectorBase<Type, Range, AllocateFunc, DeallocateFunc>, Type, Range, Compare>;

    /** @brief 16 bytes vector with a reduced range
     * The vector guarantee that it will be sorted at any given time
     * The vector must take an allocator and a deallocator functor */
    template<typename Type, auto AllocateFunc, auto DeallocateFunc, typename Compare = std::less<Type>>
    using SortedAllocatedTinyVector = SortedAllocatedVector<Type, AllocateFunc, DeallocateFunc, std::uint32_t, Compare>;
}