/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Assert helper
 */

#pragma once

#ifdef NDEBUG // Release mode

# define KUBE_DEBUG_BUILD false

// Do nothing
# define kFAssert(expression, onError) static_cast<void>(0)
# define kFAssertFallback(expression, onFallback, onError) static_cast<void>(0)
# define kFDebugThrow(exception) std::terminate();

#else // Debug mode

# define KUBE_DEBUG_BUILD true

# define kFAssert(expression, onError) if (!(expression)) onError
# define kFAssertFallback(expression, onFallback, onError) if (!(expression)) { onFallback; onError; }
# define kFDebugThrow(exception) throw exception;
#endif

/** @brief Exception helper */
#define nothrow_ndebug !KUBE_DEBUG_BUILD

/** @brief Helper to make a function noexcept only in release */
#define noexcept_ndebug noexcept(nothrow_ndebug)
