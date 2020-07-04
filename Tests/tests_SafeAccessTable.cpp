/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the single consumer concurrent queue
 */

#include <thread>
#include <gtest/gtest.h>
#include <Kube/Core/SafeAccessTable.hpp>

using namespace kF;

TEST(SafeAccessTable, Basics)
{
    SafeAccessTable<int, std::string> table;

    auto holder = table.find(42);
    ASSERT_FALSE(holder);
    table.insert(42, "123");
    holder = table.find(42);
    ASSERT_TRUE(holder);
    ASSERT_EQ(holder.value(), "123");
}
