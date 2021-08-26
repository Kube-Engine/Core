/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedAllocatedFlatVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "AllocatedFlatVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief 8 bytes vector that allocates its size and capacity on the heap
     * The vector guarantee that it will be sorted at any given time
     * The vector must take an allocator and a deallocator functor
     *
     * @tparam Type Internal type in container
     * @tparam AllocateFunc Allocator
     * @tparam DeallocateFunc Deallocator
     * @tparam Range Range of container
     * @tparam Compare Compare operator
     */
    template<typename Type, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t, typename Compare = std::less<Type>, typename CustomHeaderType = Internal::NoCustomHeaderType>
    using SortedAllocatedFlatVector = Internal::SortedVectorDetails<Internal::AllocatedFlatVectorBase<Type, Range, AllocateFunc, DeallocateFunc, CustomHeaderType>, Type, Range, Compare>;

    /** @brief 8 bytes vector using signed char with a reduced range
     * The vector guarantee that it will be sorted at any given time
     * The vector must take an allocator and a deallocator functor */
    template<typename Type, auto AllocateFunc, auto DeallocateFunc, typename Compare = std::less<Type>, typename CustomHeaderType = Internal::NoCustomHeaderType>
    using SortedAllocatedTinyFlatVector = SortedAllocatedFlatVector<Type, AllocateFunc, DeallocateFunc, std::uint32_t, Compare, CustomHeaderType>;
}