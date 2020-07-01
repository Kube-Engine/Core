/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Kube hash function
 */

#pragma once

#include <string_view>

namespace kF
{
    /** @brief The result type of the hash function */
    using HashedName = std::uint32_t;

    constexpr HashedName HashOffset = 4294967291u;

    /** @brief Compile-time string hashing */
    constexpr HashedName Hash(char const *str, std::size_t len) noexcept {
        HashedName h = HashOffset;
        for (int i = 0; i < len; ++i)
            h = 31 * h + str[i];
        return h;
    }

    /** @brief Compile-time char hashing */
    constexpr HashedName Hash(const char c) noexcept { return 31 * 4294967291L + c; }

    /** @brief Compile-time string-view hashing */
    constexpr HashedName Hash(const std::string_view &str) { return Hash(str.data(), str.length()); }

    namespace Literal
    {
        /** @brief Compile-time string hashing literal */
        constexpr HashedName operator ""_hash(char const *str, std::size_t len) { return Hash(str, len); }

        static_assert(""_hash == HashOffset);
    }
}
