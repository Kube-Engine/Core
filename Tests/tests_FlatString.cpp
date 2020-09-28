/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the single consumer concurrent queue
 */

#include <gtest/gtest.h>

#include <Kube/Core/FlatString.hpp>

using namespace kF;

TEST(FlatString, Basics)
{
    constexpr auto value = "hello world";
    char array[std::strlen(value)];
    std::memcpy(array, value, std::strlen(value));

    auto assertStringValue = [value, &array](const auto &str) {
        ASSERT_EQ(str, value);
        ASSERT_EQ(str, value);
        ASSERT_EQ(str, array);
        ASSERT_EQ(str, std::string(value));
        ASSERT_EQ(str, std::string_view(value));
    };

    // Constructors
    Core::FlatString str(value);
    assertStringValue(str);
    str.~FlatString();
    new (&str) Core::FlatString(array);
    assertStringValue(str);
    str.~FlatString();
    new (&str) Core::FlatString(std::string(value));
    assertStringValue(str);
    str.~FlatString();
    new (&str) Core::FlatString(std::string_view(value));
    assertStringValue(str);

    // Assignments
    str = nullptr;
    ASSERT_EQ(str, nullptr);
    str = value;
    assertStringValue(str);
    str = array;
    assertStringValue(str);
    str = std::string(value);
    assertStringValue(str);
    str = std::string_view(value);
}