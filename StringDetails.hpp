/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: String details
 */

#pragma once

#include <string_view>
#include <string>
#include <cstring>

#include "Utils.hpp"

namespace kF::Core::Internal
{
    template<typename Base, typename Type, std::integral Range>
        requires std::is_trivial_v<Type>
    class StringDetails;
}

/** @brief String details bring facilities to manipulate a vector as a string */
template<typename Base, typename Type, std::integral Range>
    requires std::is_trivial_v<Type>
class kF::Core::Internal::StringDetails : public Base
{
public:
    using Base::Base;
    using Base::data;
    using Base::size;
    using Base::begin;
    using Base::end;
    using Base::resize;
    using Base::insert;
    using Base::empty;
    using Base::operator bool;

    /** @brief Default constructor */
    StringDetails(void) noexcept = default;

    /** @brief Copy constructor */
    StringDetails(const StringDetails &other) noexcept = default;

    /** @brief Move constructor */
    StringDetails(StringDetails &&other) noexcept = default;

    /** @brief CString constructor */
    explicit StringDetails(const char * const cstring) noexcept { resize(cstring, cstring + SafeStrlen(cstring)); }

    /** @brief CString length constructor */
    explicit StringDetails(const char * const cstring, const std::size_t length) noexcept { resize(cstring, cstring + length); }

    /** @brief std::string constructor */
    explicit StringDetails(const std::basic_string<Type> &other) noexcept { resize(other.begin(), other.end()); }

    /** @brief std::string_view constructor */
    explicit StringDetails(const std::basic_string_view<Type> &other) noexcept { resize(other.begin(), other.end()); }

    /** @brief Destructor */
    ~StringDetails(void) noexcept = default;

    /** @brief Copy assignment */
    StringDetails &operator=(const StringDetails &other) noexcept = default;

    /** @brief Move assignment */
    StringDetails &operator=(StringDetails &&other) noexcept = default;

    /** @brief cstring assignment */
    StringDetails &operator=(const char * const cstring) noexcept { resize(cstring, cstring + SafeStrlen(cstring)); return *this; }

    /** @brief std::string assignment */
    StringDetails &operator=(const std::basic_string<Type> &other) noexcept { resize(other.begin(), other.end()); return *this; }

    /** @brief std::string_view assignment */
    StringDetails &operator=(const std::basic_string_view<Type> &other) noexcept { resize(other.begin(), other.end()); return *this; }

    /** @brief Comparison operator */
    [[nodiscard]] bool operator==(const StringDetails &other) const noexcept { return std::equal(begin(), end(), other.begin(), other.end()); }
    [[nodiscard]] bool operator!=(const StringDetails &other) const noexcept { return !operator==(other); }

    /** @brief cstring comparison operator */
    [[nodiscard]] bool operator==(const char * const cstring) const noexcept { return std::equal(begin(), end(), cstring, cstring + SafeStrlen(cstring)); }
    [[nodiscard]] bool operator!=(const char * const cstring) const noexcept { return !operator==(cstring); }

    /** @brief std::string comparison operator */
    [[nodiscard]] bool operator==(const std::basic_string<Type> &other) const noexcept { return std::equal(begin(), end(), other.begin(), other.end()); }
    [[nodiscard]] bool operator!=(const std::basic_string<Type> &other) const noexcept { return !operator==(other); }

    /** @brief std::string_view comparison operator */
    [[nodiscard]] bool operator==(const std::basic_string_view<Type> &other) const noexcept { return std::equal(begin(), end(), other.begin(), other.end()); }
    [[nodiscard]] bool operator!=(const std::basic_string_view<Type> &other) const noexcept { return !operator==(other); }

    /** @brief Get a std::string from the object */
    [[nodiscard]] std::basic_string<Type> toStdView(void) const noexcept { return std::basic_string<Type>(data(), size()); }

    /** @brief Get a std::string_view of the object */
    [[nodiscard]] std::basic_string_view<Type> toStdString(void) const noexcept { return std::basic_string_view<Type>(data(), size()); }

private:
    /** @brief Strlen but with null cstring check */
    [[nodiscard]] static std::size_t SafeStrlen(const char * const cstring) noexcept
    {
        if (!cstring) [[unlikely]]
            return 0;
        else [[likely]]
            return std::strlen(cstring);
    }
};