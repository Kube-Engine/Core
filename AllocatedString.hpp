/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedString
 */

#pragma once

#include "VectorDetails.hpp"
#include "AllocatedVector.hpp"

namespace kF::Core
{
    /**
     * @brief String that has its size, capacity and a small cache close to the data pointer
     * The string is non-null terminated
     * The string must take an allocator and a deallocator functor
     *
     * @tparam Type Type of character
     * @tparam AllocateFunc Allocator
     * @tparam DeallocateFunc Deallocator
     * @tparam Range Range of container
     */
    template<typename Type, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t>
    using AllocatedStringBase = Internal::StringDetails<AllocatedVector<Type, AllocateFunc, DeallocateFunc, Range>, Type, Range>;

    /** @brief 16 bytes String with a reduced range
     *  The string is non-null terminated
     *  The string must take an allocator and a deallocator functor */
    template<typename Type, auto AllocateFunc, auto DeallocateFunc>
    using AllocatedTinyStringBase = AllocatedStringBase<Type, AllocateFunc, DeallocateFunc, std::uint32_t>;

    /** @brief 24 bytes string
     *  The string is non-null terminated
     *  The string must take an allocator and a deallocator functor */
    template<auto AllocateFunc, auto DeallocateFunc>
    using AllocatedString = AllocatedStringBase<char, AllocateFunc, DeallocateFunc, std::size_t>;

    /** @brief 16 bytes string with a reduced range
     *  The string is non-null terminated
     *  The string must take an allocator and a deallocator functor */
    template<auto AllocateFunc, auto DeallocateFunc>
    using AllocatedTinyString = AllocatedTinyStringBase<char, AllocateFunc, DeallocateFunc>;
}