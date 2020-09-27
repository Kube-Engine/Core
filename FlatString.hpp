/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatString
 */

#include <memory>
#include <string_view>
#include <string>
#include <cstring>
#include <algorithm>

#include "FlatVector.hpp"

namespace kF::Core
{
    template<typename Type> requires std::copy_constructible<Type>
    class FlatString;
}

/** @brief Flat string is pointer-sized std::string alternative that is NOT NULL TERMINATED
 * The implementation assumes that given character Type will never throw while manipulating it
 * The implementation has same performances as std::string but comes with two weakness :
 * Because the size and capacity are stored on the heap if you wish to get the vector size and not lookup after that
 * it is slower due to memory indirection
 * Because the string don't have small optimization, it will perform worse than an std::string with short strings
*/
template<typename Type = char> requires std::copy_constructible<Type>
class kF::Core::FlatString : public kF::Core::FlatVector<Type>
{
public:
    using FlatVector<Type>::FlatVector;
    using FlatVector<Type>::data;
    using FlatVector<Type>::size;
    using FlatVector<Type>::begin;
    using FlatVector<Type>::end;
    using FlatVector<Type>::resize;
    using FlatVector<Type>::insert;

    /** @brief Default constructor */
    FlatString(void) noexcept = default;

    /** @brief Copy constructor */
    FlatString(const FlatString &other) noexcept = default;

    /** @brief Move constructor */
    FlatString(FlatString &&other) noexcept = default;

    /** @brief CString constructor */
    explicit FlatString(const char * const cstring) noexcept { resize(cstring, cstring + SafeStrlen(cstring)); }

    /** @brief CString array constructor */
    template<std::size_t Size>
    explicit FlatString(const char cstring[Size]) noexcept { resize(&cstring[0], &cstring[Size]); }

    /** @brief CString length constructor */
    explicit FlatString(const char * const cstring, const std::size_t length) noexcept { resize(cstring, cstring + length); }

    /** @brief std::string constructor */
    explicit FlatString(const std::basic_string<Type> &other) noexcept { resize(other.begin(), other.end()); }

    /** @brief std::string_view constructor */
    explicit FlatString(const std::basic_string_view<Type> &other) noexcept { resize(other.begin(), other.end()); }

    /** @brief Destructor */
    ~FlatString(void) noexcept = default;

    /** @brief Copy assignment */
    FlatString &operator=(const FlatString &other) noexcept = default;

    /** @brief Move assignment */
    FlatString &operator=(FlatString &&other) noexcept = default;

    /** @brief cstring assignment */
    FlatString &operator=(const char * const cstring) noexcept { resize(cstring, cstring + SafeStrlen(cstring)); return *this; }

    /** @brief cstring array assignment */
    template<std::size_t Size>
    FlatString &operator=(const char cstring[Size]) noexcept { resize(cstring, cstring + Size); return *this; }

    /** @brief std::string assignment */
    FlatString &operator=(const std::basic_string<Type> &other) noexcept { resize(other.begin(), other.end()); return *this; }

    /** @brief std::string_view assignment */
    FlatString &operator=(const std::basic_string_view<Type> &other) noexcept { resize(other.begin(), other.end()); return *this; }

    /** @brief Comparison operator */
    [[nodiscard]] bool operator==(const FlatString &other) const noexcept { return std::equal(begin(), end(), other.begin(), other.end()); }
    [[nodiscard]] bool operator!=(const FlatString &other) const noexcept { return !operator==(other); }

    /** @brief cstring comparison operator */
    [[nodiscard]] bool operator==(const char * const cstring) const noexcept { return std::equal(begin(), end(), cstring, cstring + SafeStrlen(cstring)); }
    [[nodiscard]] bool operator!=(const char * const cstring) const noexcept { return !operator==(cstring); }

    /** @brief cstring array comparison operator */
    template<std::size_t Size>
    [[nodiscard]] bool operator==(const char cstring[Size]) const noexcept { return std::equal(begin(), end(), cstring, cstring + Size); }
    template<std::size_t Size>
    [[nodiscard]] bool operator!=(const char cstring[Size]) const noexcept { return !operator==(cstring); }

    /** @brief std::string comparison operator */
    [[nodiscard]] bool operator==(const std::basic_string<Type> &other) const noexcept { return std::equal(begin(), end(), other.begin(), other.end()); }
    [[nodiscard]] bool operator!=(const std::basic_string<Type> &other) const noexcept { return !operator==(other); }

    /** @brief std::string_view comparison operator */
    [[nodiscard]] bool operator==(const std::basic_string_view<Type> &other) const noexcept { return std::equal(begin(), end(), other.begin(), other.end()); }
    [[nodiscard]] bool operator!=(const std::basic_string_view<Type> &other) const noexcept { return !operator==(other); }

    /** @brief Get a std::string from the object */
    [[nodiscard]] std::basic_string<Type> getStdView(void) const noexcept { return std::basic_string<Type>(data(), size()); }

    /** @brief Get a std::string_view of the object */
    [[nodiscard]] std::basic_string_view<Type> getStdString(void) const noexcept { return std::basic_string_view<Type>(data(), size()); }

private:
    [[nodiscard]] static std::size_t SafeStrlen(const char * const cstring) noexcept
    {
        if (!cstring) [[unlikely]]
            return 0;
        else [[likely]]
            return std::strlen(cstring);
    }
};