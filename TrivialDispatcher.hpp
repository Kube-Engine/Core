/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: TrivialDispatcher
 */

#pragma once

#include "TrivialFunctor.hpp"
#include "DispatcherDetails.hpp"

namespace kF::Core
{
    /** @brief Specialization of dispatcher with 'TrivialFunctor' */
    template<typename Signature, std::size_t CacheSize = CacheLineEighthSize>
    using TrivialDispatcher = DispatcherDetails<Signature, CacheSize, TrivialFunctor>;
}