/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the single consumer concurrent queue
 */

#include <gtest/gtest.h>

#include <Kube/Core/FlatVector.hpp>

using namespace kF;

TEST(FlatVector, Basics)
{
    Core::FlatVector<std::size_t> vector(0);
    ASSERT_EQ(vector.size(), 0);
    ASSERT_EQ(vector.capacity(), 0);
}

TEST(FlatVector, Push)
{
    constexpr auto count = 42ul;
    Core::FlatVector<std::size_t> vector;

    ASSERT_FALSE(vector);
    for (auto i = 0ul; i < count; ++i)
        vector.push(i);
    ASSERT_TRUE(vector);
    ASSERT_EQ(vector.size(), count);
    auto i = 0ul;
    for (const auto elem : vector) {
        ASSERT_EQ(elem, i);
        ++i;
    }
    ASSERT_EQ(i, count);
}

TEST(FlatVector, Pop)
{
    constexpr auto count = 42ul;
    Core::FlatVector<std::size_t> vector(count, 0ul);

    ASSERT_TRUE(vector);
    ASSERT_EQ(vector.size(), count);
    for (auto i = 0ul; i < count; ++i) {
        ASSERT_EQ(vector.size(), count - i);
        vector.pop();
    }
    ASSERT_EQ(vector.size(), 0);
}

TEST(FlatVector, Resize)
{
    constexpr auto str = "FlatVector is an amazing 8 bytes vector !";
    constexpr auto str2 = "Hello";
    constexpr auto count = 4ul;

    Core::FlatVector<std::string> vector(count, str);
    ASSERT_EQ(vector.size(), count);
    ASSERT_EQ(vector.capacity(), count);
    auto i = 0ul;
    for (const auto &elem : vector) {
        ASSERT_EQ(elem, str);
        ++i;
    }
    ASSERT_EQ(i, count);
    vector.resize(count, str2);
    ASSERT_EQ(vector.size(), count);
    ASSERT_EQ(vector.capacity(), count);
    i = 0ul;
    for (const auto &elem : vector) {
        ASSERT_EQ(elem, str2);
        ++i;
    }
    ASSERT_EQ(i, count);
}

TEST(FlatVector, Reserve)
{
    constexpr auto str = "FlatVector is an amazing 8 bytes vector !";
    constexpr auto count = 4ul;

    Core::FlatVector<std::string> vector;

    vector.reserve(count);
    ASSERT_EQ(vector.size(), 0);
    ASSERT_EQ(vector.capacity(), count);
    vector.resize(count, str);
    ASSERT_EQ(vector.size(), count);
    ASSERT_EQ(vector.capacity(), count);
    vector.reserve(count - 1);
    ASSERT_EQ(vector.size(), 0);
    ASSERT_EQ(vector.capacity(), count - 1);
}