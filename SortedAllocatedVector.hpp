/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sorted Vector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "AllocatedVectorBase.hpp"

namespace kF::Core
{
    template<typename Type, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t, typename Compare = std::less<Type>>
    using SortedAllocatedVector = Internal::SortedVectorDetails<Internal::AllocatedVectorBase<Type, Range, AllocateFunc, DeallocateFunc>, Type, Range, Compare>;

    template<typename Type, auto AllocateFunc, auto DeallocateFunc, typename Compare = std::less<Type>>
    using SortedAllocatedTinyVector = SortedAllocatedVector<Type, AllocateFunc, DeallocateFunc, std::uint32_t, Compare>;
}