/**
 * @ Author: Matthieu Moinvaziri
 * @ Description:
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
    [[nodiscard]] std::string FormatStdString(Args &&...args)
    {
        constexpr auto Concat = [](std::string &res, const auto &arg) {
            using ArgType = std::remove_cvref_t<decltype(arg)>;
            if constexpr ((!std::is_integral_v<ArgType> || std::is_same_v<ArgType, char>)
                    && Core::Utils::IsDetectedExact<std::string &, Internal::StringConcatable, decltype(arg)>)
                res += arg;
            else
                res += std::to_string(arg);
        };

        std::string res;
        (Concat(res, args), ...);
        return res;
    }
}