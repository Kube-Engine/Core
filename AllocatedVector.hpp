/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Vector
 */

#pragma once

#include "VectorDetails.hpp"
#include "AllocatedVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief Vector that has its size and capacity close to the data pointer
     * With default range (std::size_t), the vector takes 24 bytes
     * The vector must take an allocator and a deallocator functor
     *
     * @tparam Type Internal type in container
     * @tparam AllocateFunc Allocator
     * @tparam DeallocateFunc Deallocator
     * @tparam Range Range of container
     */
    template<typename Type, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t>
    using AllocatedVector = Internal::VectorDetails<Internal::AllocatedVectorBase<Type, Range, AllocateFunc, DeallocateFunc>, Type, Range>;

    /** @brief 16 bytes vector with a reduced range
     * The vector must take an allocator and a deallocator functor */
    template<typename Type, auto AllocateFunc, auto DeallocateFunc>
    using AllocatedTinyVector = Vector<Type, std::uint32_t>;
}