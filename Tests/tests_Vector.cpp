/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the single consumer concurrent queue
 */
#include <gtest/gtest.h>

#include <Kube/Core/Vector.hpp>

using namespace kF;

TEST(Vector, Basics)
{
    Core::Vector<std::size_t> vector;
    ASSERT_EQ(vector.size(), 0);
    ASSERT_EQ(vector.capacity(), 0);
    ASSERT_EQ(vector.cbegin(), nullptr);
    ASSERT_EQ(vector.push(42ul), 42ul);
}