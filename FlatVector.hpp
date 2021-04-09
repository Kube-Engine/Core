/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

#pragma once

#include "VectorDetails.hpp"
#include "FlatVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief 8 bytes vector that allocates its size and capacity on the heap
     *
     * @tparam Type Internal type in container
     * @tparam Range Range of container
     */
    template<typename Type, std::integral Range = std::size_t, typename CustomHeaderType = Internal::NoCustomHeaderType>
    using FlatVector = Internal::VectorDetails<Internal::FlatVectorBase<Type, Range, CustomHeaderType>, Type, Range>;

    /** @brief 8 bytes vector using signed char with a reduced range */
    template<typename Type, typename CustomHeaderType = Internal::NoCustomHeaderType>
    using TinyFlatVector = Internal::VectorDetails<Internal::FlatVectorBase<Type, std::uint32_t, CustomHeaderType>, Type, std::uint32_t>;
}
