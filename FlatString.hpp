/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatString
 */

#pragma once

#include <memory>
#include <string_view>
#include <string>
#include <cstring>
#include <algorithm>

#include "FlatVector.hpp"

namespace kF::Core
{
    template<typename Type>
    class FlatStringBase;

    using FlatString = FlatStringBase<char>;
}

/** @brief Flat string is pointer-sized std::string alternative that is NOT NULL TERMINATED
 * The implementation assumes that given character Type will never throw while manipulating it
 * The implementation comes with two weakness :
 * Because the size and capacity are stored on the heap if you wish to get the vector size and not lookup after that
 * it is slower due to memory indirection
 * Because the string don't have small optimization, it will perform worse than an std::string with short strings
*/
template<typename Type>
class kF::Core::FlatStringBase : public kF::Core::FlatVector<Type>
{
public:
    using FlatVector<Type>::FlatVector;
    using FlatVector<Type>::data;
    using FlatVector<Type>::size;
    using FlatVector<Type>::begin;
    using FlatVector<Type>::end;
    using FlatVector<Type>::resize;
    using FlatVector<Type>::insert;
    using FlatVector<Type>::empty;
    using FlatVector<Type>::operator bool;

    /** @brief Default constructor */
    FlatStringBase(void) noexcept = default;

    /** @brief Copy constructor */
    FlatStringBase(const FlatStringBase &other) noexcept = default;

    /** @brief Move constructor */
    FlatStringBase(FlatStringBase &&other) noexcept = default;

    /** @brief CString constructor */
    explicit FlatStringBase(const char * const cstring) noexcept { resize(cstring, cstring + SafeStrlen(cstring)); }

    /** @brief CString length constructor */
    explicit FlatStringBase(const char * const cstring, const std::size_t length) noexcept { resize(cstring, cstring + length); }

    /** @brief std::string constructor */
    explicit FlatStringBase(const std::basic_string<Type> &other) noexcept { resize(other.begin(), other.end()); }

    /** @brief std::string_view constructor */
    explicit FlatStringBase(const std::basic_string_view<Type> &other) noexcept { resize(other.begin(), other.end()); }

    /** @brief Destructor */
    ~FlatStringBase(void) noexcept = default;

    /** @brief Copy assignment */
    FlatStringBase &operator=(const FlatStringBase &other) noexcept = default;

    /** @brief Move assignment */
    FlatStringBase &operator=(FlatStringBase &&other) noexcept = default;

    /** @brief cstring assignment */
    FlatStringBase &operator=(const char * const cstring) noexcept { resize(cstring, cstring + SafeStrlen(cstring)); return *this; }

    /** @brief std::string assignment */
    FlatStringBase &operator=(const std::basic_string<Type> &other) noexcept { resize(other.begin(), other.end()); return *this; }

    /** @brief std::string_view assignment */
    FlatStringBase &operator=(const std::basic_string_view<Type> &other) noexcept { resize(other.begin(), other.end()); return *this; }

    /** @brief Comparison operator */
    [[nodiscard]] bool operator==(const FlatStringBase &other) const noexcept { return std::equal(begin(), end(), other.begin(), other.end()); }
    [[nodiscard]] bool operator!=(const FlatStringBase &other) const noexcept { return !operator==(other); }

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