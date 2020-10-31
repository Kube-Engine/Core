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
    vector.clear();
    ASSERT_FALSE(vector);
    for (auto i = 0ul; i < count; ++i)
        vector.push(i);
    ASSERT_TRUE(vector);
    ASSERT_EQ(vector.size(), count);
    i = 0ul;
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
    std::vector<std::string> tmp(count * 2, str);
    vector.resize(std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end()));
    ASSERT_EQ(vector.size(), count * 2);
    for (auto &elem : tmp)
        ASSERT_TRUE(elem.empty());
    for (auto &elem : vector)
        ASSERT_EQ(elem, str);
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

TEST(FlatVector, InsertIterators)
{
    std::vector<int> tmp(10, 42);
    std::vector<int> tmp2(5, 32);
    Core::FlatVector<int> vector(tmp.begin(), tmp.end());

    ASSERT_EQ(vector.size(), 10ul);
    for (auto elem : vector)
        ASSERT_EQ(elem, 42);
    vector.insert(vector.begin() + 1, tmp2.begin(), tmp2.end());
    for (auto i = 0ul; i < 5; ++i)
        ASSERT_EQ(vector[1 + i], 32);
    vector.insert(vector.end(), { 45, 46 });
    ASSERT_EQ(vector.at(vector.size() - 2), 45);
    ASSERT_EQ(vector.back(), 46);
}

TEST(FlatVector, InsertFill)
{
    Core::FlatVector<int> vector;

    vector.insert(vector.begin(), 2, 42);
    ASSERT_EQ(vector.size(), 2);
    for (auto &elem : vector)
        ASSERT_EQ(elem, 42);
    vector.insert(vector.begin(), 1, 32);
    ASSERT_EQ(vector.front(), 32);
    vector.insert(vector.end(), 1, 32);
    ASSERT_EQ(vector.back(), 32);
    vector.insert(vector.end(), 42, 32);
    for (auto i = 4u; i < vector.size<false>(); ++i)
        ASSERT_EQ(vector[i], 32);
}

TEST(FlatVector, Clear)
{
    constexpr auto count = 42;
    constexpr auto value1 = 24;
    constexpr auto value2 = 42;
    Core::FlatVector<int> vector(count, value1);
    auto *data = vector.data();
    for (auto i = 0u; i < count; ++i)
        ASSERT_EQ(data[i], value1);
    vector.clear();
    vector.insert(vector.begin(), count, value2);
    for (auto i = 0u; i < count; ++i)
            ASSERT_EQ(data[i], value2);
    vector.clear();
    vector.insert(vector.begin(), count * 2, value1);
    ASSERT_NE(vector.data(), data);
}

TEST(FlatVector, Erase)
{
    constexpr auto count = 10;

    constexpr auto Get = [] {
        Core::FlatVector<int> vector(count);
        for (auto i = 0; i < count; ++i)
            vector[i] = i;
    };

    {
        auto vector = Get();
        vector.erase(vector.begin(), vector.end());
        ASSERT_EQ(vector.size(), 0);
    }
    {
        auto vector = Get();
        vector.erase(vector.end() - count / 2, vector.end());
        ASSERT_EQ(vector.size(), count / 2);
        for (auto i = 0; auto elem : vector)
            ASSERT_EQ(elem == i++);
        vector.erase(vector.begin(), vector.size() - 1);
        ASSERT_EQ(vector.size(), 1);
        ASSERT_EQ(vector.front(), count / 2 - 1);
        vector.erase(vector.begin());
        ASSERT_EQ(vector.size(), 0);
    }
}