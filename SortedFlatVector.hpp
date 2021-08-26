/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedFlatVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "FlatVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief 8 bytes vector that allocates its size and capacity on the heap
     * The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam Range Range of container
     * @tparam Compare Compare operator
     */
    template<typename Type, std::integral Range = std::size_t, typename Compare = std::less<Type>, typename CustomHeaderType = Internal::NoCustomHeaderType>
    using SortedFlatVector = Internal::SortedVectorDetails<Internal::FlatVectorBase<Type, Range, CustomHeaderType>, Type, Range, Compare>;

    /** @brief 8 bytes vector using signed char with a reduced range
     * The vector guarantee that it will be sorted at any given time */
    template<typename Type, typename Compare = std::less<Type>, typename CustomHeaderType = Internal::NoCustomHeaderType>
    using SortedTinyFlatVector = SortedFlatVector<Type, std::uint32_t, Compare, CustomHeaderType>;
}