/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sorted SmallVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "AllocatedSmallVectorBase.hpp"

namespace kF::Core
{
    template<typename Type, std::size_t OptimizedCapacity, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t, typename Compare = std::less<Type>>
    using SortedAllocatedSmallVector = Internal::SortedVectorDetails<Internal::AllocatedSmallVectorBase<Type, OptimizedCapacity, AllocateFunc, DeallocateFunc, Range>, Type, Range, Compare, true>;

    template<typename Type, std::size_t OptimizedCapacity, auto AllocateFunc, auto DeallocateFunc, typename Compare = std::less<Type>>
    using SortedAllocatedTinySmallVector = SortedAllocatedSmallVector<Type, OptimizedCapacity, AllocateFunc, DeallocateFunc, std::uint32_t, Compare>;
}