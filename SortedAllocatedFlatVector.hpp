/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sorted FlatVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "AllocatedFlatVectorBase.hpp"

namespace kF::Core
{
    template<typename Type, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t, typename Compare = std::less<Type>>
    using SortedAllocatedFlatVector = Internal::SortedVectorDetails<Internal::AllocatedFlatVectorBase<Type, Range, AllocateFunc, DeallocateFunc>, Type, Range, Compare>;

    template<typename Type, auto AllocateFunc, auto DeallocateFunc, typename Compare = std::less<Type>>
    using SortedAllocatedTinyFlatVector = SortedAllocatedFlatVector<Type, AllocateFunc, DeallocateFunc, std::uint32_t, Compare>;
}