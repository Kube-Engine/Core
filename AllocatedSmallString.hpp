/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: AllocatedSmallString
 */

#pragma once

#include "AllocatedSmallVector.hpp"
#include "StringDetails.hpp"

namespace kF::Core
{
    /**
     * @brief String that has its size, capacity and a small cache close to the data pointer
     * The string is non-null terminated
     * The string must take an allocator and a deallocator functor
     *
     * @tparam Type Type of character
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam AllocateFunc Allocator
     * @tparam DeallocateFunc Deallocator
     * @tparam Range Range of container
     */
    template<typename Type, std::size_t OptimizedCapacity, auto AllocateFunc, auto DeallocateFunc, std::integral Range = std::size_t>
    using AllocatedSmallStringBase = Internal::StringDetails<AllocatedSmallVector<Type, OptimizedCapacity, AllocateFunc, DeallocateFunc, Range>, Type, Range>;

    /** @brief Small optimized string with a reduced range
     *  The string is non-null terminated
     *  The string must take an allocator and a deallocator functor */
    template<typename Type, std::size_t OptimizedCapacity, auto AllocateFunc, auto DeallocateFunc>
    using AllocatedTinySmallStringBase = AllocatedSmallStringBase<Type, OptimizedCapacity, AllocateFunc, DeallocateFunc, std::uint32_t>;

    /** @brief 32 bytes small optimized string (cache of 8 bytes)
     *  The string is non-null terminated
     *  The string must take an allocator and a deallocator functor */
    template<auto AllocateFunc, auto DeallocateFunc>
    using AllocatedSmallString = AllocatedSmallStringBase<char, CacheLineEighthSize, AllocateFunc, DeallocateFunc, std::size_t>;

    /** @brief 32 bytes small optimized string (cache of 16 bytes) with a reduced range
     *  The string is non-null terminated
     *  The string must take an allocator and a deallocator functor */
    template<auto AllocateFunc, auto DeallocateFunc>
    using AllocatedTinySmallString = AllocatedTinySmallStringBase<char, CacheLineQuarterSize, AllocateFunc, DeallocateFunc>;
}
