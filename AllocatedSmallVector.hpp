/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Small Vector
 */

#pragma once

#include "VectorDetails.hpp"
#include "AllocatedSmallVectorBase.hpp"

namespace kF::Core
{
    template<typename Type, std::size_t OptimizedCapacity, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t>
    using AllocatedSmallVector = Internal::VectorDetails<Internal::AllocatedSmallVectorBase<Type, OptimizedCapacity, AllocateFunc, DeallocateFunc, Range>, Type, Range, true>;

    template<typename Type, std::size_t OptimizedCapacity, auto AllocateFunc, auto DeallocateFunc>
    using AllocatedTinySmallVector = AllocatedSmallVector<Type, OptimizedCapacity, AllocateFunc, DeallocateFunc, std::uint32_t>;
}
