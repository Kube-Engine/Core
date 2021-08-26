/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedSmallVector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "SmallVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size, capacity and a small cache close to the data pointer
     * The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Range Range of container
     * @tparam Compare Compare operator
     */
    template<typename Type, std::size_t OptimizedCapacity, std::integral Range = std::size_t, typename Compare = std::less<Type>>
    using SortedSmallVector = Internal::SortedVectorDetails<Internal::SmallVectorBase<Type, OptimizedCapacity, Range>, Type, Range, Compare, true>;

    /** @brief Small optimized vector with a reduced range
     * The vector guarantee that it will be sorted at any given time */
    template<typename Type, std::size_t OptimizedCapacity, typename Compare = std::less<Type>>
    using SortedTinySmallVector = SortedSmallVector<Type, OptimizedCapacity, std::uint32_t, Compare>;
}