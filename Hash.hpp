/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Compile time hash function
 */

#pragma once

#include <string_view>

namespace kF
{
    /** @brief The result type of the hash function */
    using HashedName = std::uint32_t;

    /** @brief The hash begin offset */
    constexpr HashedName HashOffset = 4294967291u;

    /** @brief Compile-time string hashing */
    [[nodiscard]] constexpr HashedName Hash(char const * const str, const std::size_t len) noexcept {
        HashedName hash = HashOffset;
        for (std::size_t i = 0ul; i < len; ++i)
            hash = 31u * hash + str[i];
        return hash;
    }

    /** @brief Compile-time char hashing */
    [[nodiscard]] constexpr HashedName Hash(const char c) noexcept
        { return 31u * HashOffset + c; }

    /** @brief Compile-time string-view hashing */
    [[nodiscard]] constexpr HashedName Hash(const std::string_view &str) noexcept
        { return Hash(str.data(), str.length()); }


    /** @brief Compile-time string hashing */
    [[nodiscard]] constexpr HashedName ContinueHash(HashedName hash, char const * const str, const std::size_t len) noexcept {
        for (std::size_t i = 0ul; i < len; ++i)
            hash = 31u * hash + str[i];
        return hash;
    }

    /** @brief Compile-time char hashing */
    [[nodiscard]] constexpr HashedName ContinueHash(HashedName hash, const char c) noexcept
        { return 31u * hash + c; }

    /** @brief Compile-time string-view hashing */
    [[nodiscard]] constexpr HashedName ContinueHash(HashedName hash, const std::string_view &str) noexcept
        { return ContinueHash(hash, str.data(), str.length()); }

    namespace Literal
    {
        /** @brief Compile-time string hashing literal */
        [[nodiscard]] constexpr HashedName operator ""_hash(char const *str, std::size_t len) noexcept { return Hash(str, len); }

        static_assert(""_hash == HashOffset, "There is an error in compile-time hashing algorithm");
    }
    static_assert(Hash("1234") == ContinueHash(ContinueHash(Hash('1'), "2"), "34"), "There is an error in compile-time hashing algorithm");
}
