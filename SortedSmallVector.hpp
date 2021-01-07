/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sorted SmallVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "SmallVectorBase.hpp"

namespace kF::Core
{
    template<typename Type, std::size_t OptimizedCapacity, std::integral Range = std::size_t, typename Compare = std::less<Type>>
    using SortedSmallVector = Internal::SortedVectorDetails<Internal::SmallVectorBase<Type, OptimizedCapacity, Range>, Type, Range, Compare, true>;

    template<typename Type, std::size_t OptimizedCapacity, typename Compare = std::less<Type>>
    using TinySortedSmallVector = SortedSmallVector<Type, OptimizedCapacity, std::uint32_t, Compare>;
}