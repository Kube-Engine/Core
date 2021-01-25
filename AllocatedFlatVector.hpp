/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

#pragma once

#include "VectorDetails.hpp"
#include "AllocatedFlatVectorBase.hpp"

namespace kF::Core
{
    /**
     * @brief 8 bytes vector that allocates its size and capacity on the heap
     * The vector must take an allocator and a deallocator functor
     *
     * @tparam Type Internal type in container
     * @tparam AllocateFunc Allocator
     * @tparam DeallocateFunc Deallocator
     * @tparam Range Range of container
     */
    template<typename Type, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t>
    using AllocatedFlatVector = Internal::VectorDetails<Internal::AllocatedFlatVectorBase<Type, Range, AllocateFunc, DeallocateFunc>, Type, Range>;

    /** @brief 8 bytes vector using signed char with a reduced range
     * The vector must take an allocator and a deallocator functor */
    template<typename Type, auto AllocateFunc, auto DeallocateFunc>
    using AllocatedTinyFlatVector = AllocatedFlatVector<Type, AllocateFunc, DeallocateFunc, std::uint32_t>;
}
