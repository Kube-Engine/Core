/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Small Vector
 */

#pragma once

#include "VectorDetails.hpp"
#include "SmallVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size, capacity and a small cache close to the data pointer
     *
     * @tparam Type Internal type in container
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Range Range of container
     */
    template<typename Type, std::size_t OptimizedCapacity, std::integral Range = std::size_t>
    using SmallVector = Internal::VectorDetails<Internal::SmallVectorBase<Type, OptimizedCapacity, Range>, Type, Range, true>;

    /** @brief Small optimized vector with a reduced range */
    template<typename Type, std::size_t OptimizedCapacity>
    using TinySmallVector = SmallVector<Type, OptimizedCapacity, std::uint32_t>;
}
