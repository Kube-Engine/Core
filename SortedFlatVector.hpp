/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sorted FlatVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "FlatVectorBase.hpp"

namespace kF::Core
{
    template<typename Type, std::integral Range = std::size_t, typename Compare = std::less<Type>>
    using SortedFlatVector = Internal::SortedVectorDetails<Internal::FlatVectorBase<Type, Range>, Type, Range, Compare>;

    template<typename Type, typename Compare = std::less<Type>>
    using TinySortedFlatVector = SortedFlatVector<Type, std::uint32_t, Compare>;
}