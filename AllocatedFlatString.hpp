/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

#pragma once

#include "FlatString.hpp"
#include "AllocatedFlatVector.hpp"

namespace kF::Core
{
    /**
     * @brief 8 bytes vector that allocates its size and capacity on the heap
     * The string is non-null terminated
     * The string must take an allocator and a deallocator functor
     *
     * @tparam Type Internal type in container
     * @tparam AllocateFunc Allocator
     * @tparam DeallocateFunc Deallocator
     * @tparam Range Range of container
     */
    template<typename Type, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t>
    using AllocatedFlatStringBase = Internal::StringDetails<AllocatedFlatVector<Type, AllocateFunc, DeallocateFunc, Range>, Type, Range>;

    /** @brief 8 bytes string using signed char and size_t range
     *  The string is non-null terminated
     *  The string must take an allocator and a deallocator functor */
    template<auto AllocateFunc, auto DeallocateFunc>
    using AllocatedFlatString = AllocatedFlatStringBase<char, AllocateFunc, DeallocateFunc, std::size_t>;

    /** @brief 8 bytes string using signed char with a reduced range
     *  The string is non-null terminated
     *  The string must take an allocator and a deallocator functor */
    template<auto AllocateFunc, auto DeallocateFunc>
    using AllocatedTinyFlatString = AllocatedFlatStringBase<char, AllocateFunc, DeallocateFunc, std::uint32_t>;
}
