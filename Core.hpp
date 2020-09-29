/**
 * @ Author: Matthieu Moinvaziri
 * @ Description:
 */

#pragma once

#include <string>

#include "Version.hpp"

namespace kF
{
    inline constexpr Version KubeVersion(0, 1, 0);

    namespace Literal
    {
        inline std::basic_string<char> operator""_str(const char *data, const std::size_t size) { return std::basic_string<char>{ data, size }; }
    }
}