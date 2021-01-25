/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the single consumer concurrent queue
 */

#include <gtest/gtest.h>

#include <memory_resource>

#include <Kube/Core/String.hpp>
#include <Kube/Core/AllocatedString.hpp>
#include <Kube/Core/FlatString.hpp>
#include <Kube/Core/AllocatedFlatString.hpp>
#include <Kube/Core/SmallString.hpp>
#include <Kube/Core/AllocatedSmallString.hpp>

#define GENERATE_STRING_TESTS(String, ...) \
using String##Class = String<char __VA_OPT__(,) __VA_ARGS__>; \
TEST(String, Basics) \
{ \
    constexpr const char *value = "hello world"; \
    char array[] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd' , '\0' }; \
 \
    auto assertStringValue = [value, &array](const String##Class &str) { \
        ASSERT_EQ(str, value); \
        ASSERT_EQ(str, array); \
        ASSERT_EQ(str, std::string(value)); \
        ASSERT_EQ(str, std::string_view(value)); \
    }; \
 \
    String##Class str(value); \
    assertStringValue(str); \
    str.release(); \
    new (&str) String##Class(array); \
    assertStringValue(str); \
    str.release(); \
    new (&str) String##Class(std::string(value)); \
    assertStringValue(str); \
    str.release(); \
    new (&str) String##Class(std::string_view(value)); \
    assertStringValue(str); \
 \
    str = nullptr; \
    ASSERT_EQ(str, nullptr); \
    str = value; \
    assertStringValue(str); \
    str = array; \
    assertStringValue(str); \
    str = std::string(value); \
    assertStringValue(str); \
    str = std::string_view(value); \
}

using namespace kF::Core;

static std::pmr::unsynchronized_pool_resource Pool;

static void *DefaultAlloc(const std::size_t bytes, const std::size_t alignment)
{
    return Pool.allocate(bytes, alignment);
}

static void DefaultDealloc(void * const data, const std::size_t bytes, const std::size_t alignment)
{
    Pool.deallocate(data, bytes, alignment);
}

GENERATE_STRING_TESTS(StringBase)
GENERATE_STRING_TESTS(AllocatedStringBase, &DefaultAlloc, &DefaultDealloc)
GENERATE_STRING_TESTS(FlatStringBase)
GENERATE_STRING_TESTS(AllocatedFlatStringBase, &DefaultAlloc, &DefaultDealloc)
GENERATE_STRING_TESTS(SmallStringBase, 4ul)
GENERATE_STRING_TESTS(AllocatedSmallStringBase, 4ul, &DefaultAlloc, &DefaultDealloc)
