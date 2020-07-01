/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Assert helper
 */

#pragma once

#ifdef NDEBUG // Release mode

# define KUBE_DEBUG_BUILD false

#define kFAssert(expression, onError) static_cast<void>(0)

#else // Debug mode

# define KUBE_DEBUG_BUILD true

# define kFAssert(expression, onError) if (!(expression)) onError

#endif

/** @brief Helper to make a function noexcept only in release */
#define noexcept_ndebug noexcept(!KUBE_DEBUG_BUILD)
