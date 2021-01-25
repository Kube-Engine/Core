/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Sorted Vector
 */

#pragma once

#include "SortedVectorDetails.hpp"
#include "VectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size and capacity close to the data pointer
     * With default range (std::size_t), the vector takes 24 bytes
     * The vector guarantee that it will be sorted at any given time
     *
     * @tparam Type Internal type in container
     * @tparam Range Range of container
     * @tparam Compare Compare operator
     */
    template<typename Type, std::integral Range = std::size_t, typename Compare = std::less<Type>>
    using SortedVector = Internal::SortedVectorDetails<Internal::VectorBase<Type, Range>, Type, Range, Compare>;

    /** @brief 16 bytes vector with a reduced range
     * The vector guarantee that it will be sorted at any given time */
    template<typename Type, typename Compare = std::less<Type>>
    using SortedTinyVector = SortedVector<Type, std::uint32_t, Compare>;
}