/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

#pragma once

#include "VectorDetails.hpp"
#include "FlatVectorBase.hpp"

namespace kF::Core
{
    template<typename Type, std::integral Range = std::size_t>
    using FlatVector = Internal::VectorDetails<Internal::FlatVectorBase<Type, Range>, Type, Range>;
}
