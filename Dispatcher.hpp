/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Trivial dispatcher
 */

#pragma once

#include "Functor.hpp"
#include "DispatcherDetails.hpp"

namespace kF::Core
{
    /** @brief Specialization of dispatcher with trivial functors */
    template<typename Signature, std::size_t CacheSize = CacheLineQuarterSize>
    using Dispatcher = DispatcherDetails<Signature, CacheSize, Functor>;
}