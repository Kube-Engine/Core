/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Vector
 */

#pragma once

#include "VectorDetails.hpp"
#include "VectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size and capacity close to the data pointer
     *  With default range (std::size_t), the vector takes 24 bytes
     *
     * @tparam Type Internal type in container
     * @tparam Range Range of container
     */
    template<typename Type, std::integral Range = std::size_t>
    using Vector = Internal::VectorDetails<Internal::VectorBase<Type, Range>, Type, Range>;

    /** @brief 16 bytes vector with a reduced range */
    template<typename Type>
    using TinyVector = Vector<Type, std::uint32_t>;
}