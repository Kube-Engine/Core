/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Vector
 */

#pragma once

#include "VectorDetails.hpp"
#include "AllocatedVectorBase.hpp"

namespace kF::Core
{
    template<typename Type, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t>
    using AllocatedVector = Internal::VectorDetails<Internal::AllocatedVectorBase<Type, Range, AllocateFunc, DeallocateFunc>, Type, Range>;

    template<typename Type, auto AllocateFunc, auto DeallocateFunc>
    using AllocatedTinyVector = Vector<Type, std::uint32_t>;
}