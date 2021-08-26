/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SmallString
 */

#pragma once

#include "SmallVector.hpp"
#include "StringDetails.hpp"

namespace kF::Core
{
    /**
     * @brief String that has its size, capacity and a small cache close to the data pointer
     *  The string is non-null terminated
     *
     * @tparam Type Type of character
     * @tparam OptimizedCapacity Count of element in the optimized cache
     * @tparam Range Range of container
     */
    template<typename Type, std::size_t OptimizedCapacity, std::integral Range = std::size_t>
    using SmallStringBase = Internal::StringDetails<SmallVector<Type, OptimizedCapacity, Range>, Type, Range>;

    /** @brief Small optimized string with a reduced range
     *  The string is non-null terminated */
    template<typename Type, std::size_t OptimizedCapacity>
    using TinySmallStringBase = SmallStringBase<Type, OptimizedCapacity, std::uint32_t>;

    /** @brief 32 bytes small optimized string (cache of 8 bytes)
     *  The string is non-null terminated */
    using SmallString = SmallStringBase<char, CacheLineEighthSize, std::size_t>;

    /** @brief 32 bytes small optimized string (cache of 16 bytes) with a reduced range
     *  The string is non-null terminated */
    using TinySmallString = TinySmallStringBase<char, CacheLineQuarterSize>;
}

static_assert_sizeof_half_cacheline(kF::Core::SmallString);
static_assert_sizeof_half_cacheline(kF::Core::TinySmallString);
