/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Vector unit tests
 */

#include <gtest/gtest.h>

#include <Kube/Core/Vector.hpp>
#include <Kube/Core/FlatVector.hpp>
#include <Kube/Core/SmallVector.hpp>

#define GENERATE_VECTOR_TESTS(Vector, ...) \
TEST(Vector, Basics) \
{ \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> vector(0); \
    ASSERT_EQ(vector.size(), 0); \
    ASSERT_EQ(vector.capacity(), 0); \
} \
 \
TEST(Vector, Semantics) \
{ \
    for (auto i = 1; i < 15; ++i) { \
        Vector<std::string __VA_OPT__(,) __VA_ARGS__> vector(i, "Hello World 123456789"); \
        auto copy1(vector); \
        auto copy2 = copy1; \
        auto tmp1 = copy2; \
        auto tmp2 = tmp1; \
        auto move1(std::move(tmp1)); \
        auto move2 = std::move(tmp2); \
        ASSERT_EQ(vector, copy1); \
        ASSERT_EQ(vector, copy2); \
        ASSERT_EQ(vector, move1); \
        ASSERT_EQ(vector, move2); \
        ASSERT_NE(vector, tmp1); \
        ASSERT_NE(vector, tmp2); \
    } \
} \
 \
TEST(Vector, Push) \
{ \
    constexpr auto count = 42ul; \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> vector; \
 \
    ASSERT_FALSE(vector); \
    for (auto i = 0ul; i < count; ++i) { \
        ASSERT_EQ(vector.push(i), i); \
        ASSERT_EQ(vector.size(), i + 1); \
    } \
    ASSERT_TRUE(vector); \
    ASSERT_EQ(vector.size(), count); \
    auto i = 0ul; \
    for (const auto elem : vector) { \
        ASSERT_EQ(elem, i); \
        ++i; \
    } \
    ASSERT_EQ(i, count); \
    vector.clear(); \
    ASSERT_FALSE(vector); \
    for (auto i = 0ul; i < count; ++i) \
        vector.push(i); \
    ASSERT_TRUE(vector); \
    ASSERT_EQ(vector.size(), count); \
    i = 0ul; \
    for (const auto elem : vector) { \
        ASSERT_EQ(elem, i); \
        ++i; \
    } \
    ASSERT_EQ(i, count); \
} \
 \
TEST(Vector, Pop) \
{ \
    constexpr auto count = 42ul; \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> vector(count, 0ul); \
 \
    ASSERT_TRUE(vector); \
    ASSERT_EQ(vector.size(), count); \
    for (auto i = 0ul; i < count; ++i) { \
        ASSERT_EQ(vector.size(), count - i); \
        vector.pop(); \
    } \
    ASSERT_EQ(vector.size(), 0); \
} \
 \
TEST(Vector, NullResize) \
{ \
    Vector<int __VA_OPT__(,) __VA_ARGS__> vector(0); \
    ASSERT_EQ(vector.size(), 0); \
    ASSERT_EQ(vector.capacity(), 0); \
    vector.resize(0, 0); \
    ASSERT_EQ(vector.size(), 0); \
    ASSERT_EQ(vector.capacity(), 0); \
} \
 \
TEST(Vector, Resize) \
{ \
    constexpr auto str = "Vector is an amazing 8 bytes vector !"; \
    constexpr auto str2 = "Hello"; \
    constexpr auto count = 4ul; \
 \
    Vector<std::string __VA_OPT__(,) __VA_ARGS__> vector(count, str); \
    ASSERT_EQ(vector.size(), count); \
    ASSERT_EQ(vector.capacity(), count); \
    auto i = 0ul; \
    for (const auto &elem : vector) { \
        ASSERT_EQ(elem, str); \
        ++i; \
    } \
    ASSERT_EQ(i, count); \
    vector.resize(count, str2); \
    ASSERT_EQ(vector.size(), count); \
    ASSERT_EQ(vector.capacity(), count); \
    i = 0ul; \
    for (const auto &elem : vector) { \
        ASSERT_EQ(elem, str2); \
        ++i; \
    } \
    ASSERT_EQ(i, count); \
    std::vector<std::string> tmp(count * 2, str); \
    vector.resize(std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end())); \
    ASSERT_EQ(vector.size(), count * 2); \
    for (auto &elem : tmp) \
        ASSERT_TRUE(elem.empty()); \
    for (auto &elem : vector) \
        ASSERT_EQ(elem, str); \
} \
 \
TEST(Vector, Reserve) \
{ \
    constexpr auto str = "Vector is an amazing 8 bytes vector !"; \
    constexpr auto count = 4ul; \
 \
    Vector<std::string __VA_OPT__(,) __VA_ARGS__> vector; \
 \
    vector.reserve(count); \
    ASSERT_EQ(vector.size(), 0); \
    ASSERT_EQ(vector.capacity(), count); \
    vector.resize(count, str); \
    ASSERT_EQ(vector.size(), count); \
    ASSERT_EQ(vector.capacity(), count); \
    vector.reserve(count - 1); \
    ASSERT_EQ(vector.size(), count); \
    ASSERT_EQ(vector.capacity(), count); \
} \
 \
TEST(Vector, InsertIterators) \
{ \
    std::vector<int> tmp(10, 42); \
    std::vector<int> tmp2(5, 32); \
    Vector<int __VA_OPT__(,) __VA_ARGS__> vector(tmp.begin(), tmp.end()); \
 \
    ASSERT_EQ(vector.size(), 10ul); \
    for (auto elem : vector) \
        ASSERT_EQ(elem, 42); \
    vector.insert(vector.begin() + 1, tmp2.begin(), tmp2.end()); \
    for (auto i = 0ul; i < 5; ++i) \
        ASSERT_EQ(vector[1 + i], 32); \
    vector.insert(vector.end(), { 45, 46 }); \
    ASSERT_EQ(vector.at(vector.size() - 2), 45); \
    ASSERT_EQ(vector.back(), 46); \
} \
 \
TEST(Vector, InsertFill) \
{ \
    Vector<int __VA_OPT__(,) __VA_ARGS__> vector; \
 \
    vector.insert(vector.begin(), 2, 42); \
    ASSERT_EQ(vector.size(), 2); \
    for (auto &elem : vector) \
        ASSERT_EQ(elem, 42); \
    vector.insert(vector.begin(), 1, 32); \
    ASSERT_EQ(vector.front(), 32); \
    vector.insert(vector.end(), 1, 32); \
    ASSERT_EQ(vector.back(), 32); \
    vector.insert(vector.end(), 42, 32); \
    for (auto i = 4u; i < vector.size(); ++i) \
        ASSERT_EQ(vector[i], 32); \
} \
 \
TEST(Vector, Clear) \
{ \
    constexpr auto count = 42; \
    constexpr auto value1 = 24; \
    constexpr auto value2 = 42; \
    Vector<int __VA_OPT__(,) __VA_ARGS__> vector(count, value1); \
    auto *data = vector.data(); \
    for (auto i = 0u; i < count; ++i) \
        ASSERT_EQ(data[i], value1); \
    vector.clear(); \
    vector.insert(vector.begin(), count, value2); \
    for (auto i = 0u; i < count; ++i) \
            ASSERT_EQ(data[i], value2); \
    vector.clear(); \
    vector.insert(vector.begin(), count * 2, value1); \
    ASSERT_NE(vector.data(), data); \
} \
 \
TEST(Vector, Erase) \
{ \
    constexpr auto count = 10ul; \
 \
    constexpr auto Get = [](const std::size_t count) { \
        Vector<int __VA_OPT__(,) __VA_ARGS__> vector(count); \
        for (auto i = 0u; i < count; ++i) \
            vector[i] = i; \
        return vector; \
    }; \
 \
    { \
        auto vector = Get(count); \
        vector.erase(vector.begin(), vector.end()); \
        ASSERT_EQ(vector.size(), 0); \
    } \
    { \
        auto vector = Get(count); \
        vector.erase(vector.end() - count / 2, vector.end()); \
        ASSERT_EQ(vector.size(), count / 2); \
        auto i = 0u; \
        for (auto elem : vector) { \
            ASSERT_EQ(elem, i++); \
        } \
        vector.erase(vector.begin(), vector.size() - 1); \
        ASSERT_EQ(vector.size(), 1); \
        ASSERT_EQ(vector.front(), count / 2 - 1); \
        vector.erase(vector.begin()); \
        ASSERT_EQ(vector.size(), 0); \
    } \
} \
 \
TEST(Vector, Find) \
{ \
    constexpr auto count = 42ul; \
    Vector<std::size_t __VA_OPT__(,) __VA_ARGS__> vector; \
 \
    ASSERT_FALSE(vector); \
    for (auto i = 0ul; i < count; ++i) { \
        vector.push(i); \
    } \
    ASSERT_EQ(vector.find(0), vector.begin()); \
    ASSERT_EQ(vector.find(41), vector.end() - 1); \
    ASSERT_EQ(vector.find(42), vector.end()); \
    ASSERT_EQ(vector.find([](std::size_t x) { return x == 15; }), vector.begin() + 15); \
    ASSERT_EQ(vector.find([](const std::size_t x) { return x == 15; }), vector.begin() + 15); \
    ASSERT_EQ(vector.find([](const std::size_t &x) { return x == 15; }), vector.begin() + 15); \
    ASSERT_EQ(vector.find([](std::size_t &x) { return ++x == 42; }), vector.end() - 1); \
}

using namespace kF::Core;

GENERATE_VECTOR_TESTS(Vector)
GENERATE_VECTOR_TESTS(FlatVector)
GENERATE_VECTOR_TESTS(SmallVector, 4)

TEST(SmallVector, SmallOptimizationInsertRange)
{
    std::unique_ptr<int> data[] = {
        std::make_unique<int>(0), std::make_unique<int>(1),
        std::make_unique<int>(2), std::make_unique<int>(3)
    };
    SmallVector<std::unique_ptr<int>, 4> vector(
        std::make_move_iterator(std::begin(data)),
        std::make_move_iterator(std::end(data))
    );

    ASSERT_TRUE(vector.isCacheUsed());
    for (auto i = 0; i < 4; ++i)
        ASSERT_EQ(*vector.at(i), i);

    vector.push(std::make_unique<int>(4));
    ASSERT_FALSE(vector.isCacheUsed());
    for (auto i = 0; i < 5; ++i)
        ASSERT_EQ(*vector.at(i), i);
}

TEST(SmallVector, SmallOptimizationPush)
{
    constexpr auto PushTest = [](auto &vector, const int value, const bool isCacheUsed) {
        vector.push(std::make_unique<int>(value));
        ASSERT_EQ(vector.isCacheUsed(), isCacheUsed);
        for (auto i = 0; i <= value; ++i)
            ASSERT_EQ(*vector.at(i), i);
    };
    SmallVector<std::unique_ptr<int>, 4> vector;

    PushTest(vector, 0, true);
    PushTest(vector, 1, true);
    PushTest(vector, 2, true);
    PushTest(vector, 3, true);

    PushTest(vector, 4, false);
}