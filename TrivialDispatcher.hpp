/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Trivial dispatcher
 */

#pragma once

#include "TrivialFunctor.hpp"
#include "DispatcherDetails.hpp"

namespace kF::Core
{
    /** @brief Specialization of dispatcher with trivial functors */
    template<typename Signature, std::size_t CacheSize = CacheLineEighthSize>
    using TrivialDispatcher = DispatcherDetails<Signature, CacheSize, TrivialFunctor>;
}