/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: String literal
 */

#pragma once

#include <string>

#include "Utils.hpp"

namespace kF::Literal
{
    using std::string_literals::operator ""s;

    namespace Internal
    {
        template<typename Type>
        using StringConcatable = decltype(std::declval<std::string>() += std::declval<Type>());
    }

    /** @brief Helper used to easily format a std::string from anything that is a literal or implement 'std::to_string' */
    template<typename ...Args>
    [[nodiscard]] std::string FormatStdString(Args &&...args);
}

#include "StringLiteral.ipp"