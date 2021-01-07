/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Small Vector
 */

#pragma once

#include "VectorDetails.hpp"
#include "SmallVectorBase.hpp"

namespace kF::Core
{
    template<typename Type, std::size_t OptimizedCapacity, std::integral Range = std::size_t>
    using SmallVector = Internal::VectorDetails<Internal::SmallVectorBase<Type, OptimizedCapacity, Range>, Type, Range, true>;

    template<typename Type, std::size_t OptimizedCapacity>
    using TinySmallVector = SmallVector<Type, OptimizedCapacity, std::uint32_t>;
}
