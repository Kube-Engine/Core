/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the single consumer concurrent queue
 */

#include <thread>
#include <gtest/gtest.h>
#include <Kube/Core/SafeQueue.hpp>

using namespace kF;

TEST(SafeQueue, Basics)
{
    SafeQueue<int> queue;

    { // Produce
        auto producer = queue.acquireProducer();
        producer.data().emplace_back(0);
        producer.data().emplace_back(2);
        producer.data().emplace_back(4);
    }
    { // Consume
        auto consumer = queue.acquireConsumer();
        ASSERT_TRUE(consumer);
        ASSERT_EQ(consumer.data().size(), 3);
        ASSERT_EQ(consumer.data()[0], 0);
        ASSERT_EQ(consumer.data()[1], 2);
        ASSERT_EQ(consumer.data()[2], 4);
    }
}
