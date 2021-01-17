/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

#pragma once

#include "VectorDetails.hpp"
#include "AllocatedFlatVectorBase.hpp"

namespace kF::Core
{
    template<typename Type, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t>
    using AllocatedFlatVector = Internal::VectorDetails<Internal::AllocatedFlatVectorBase<Type, Range, AllocateFunc, DeallocateFunc>, Type, Range>;
}
