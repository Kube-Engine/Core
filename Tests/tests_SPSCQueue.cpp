/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the SPSC Queue
 */

#include <thread>

#include <gtest/gtest.h>

#include <Kube/Core/SPSCQueue.hpp>

using namespace kF;

constexpr auto LongStr = "123456789123456789";
constexpr auto ShortStr = "12345";

TEST(SPSCQueue, SinglePushPop)
{
    constexpr std::size_t queueSize = 8;

    Core::SPSCQueue<std::string> queue(queueSize);

    for (auto i = 0; i < queueSize; ++i)
        ASSERT_TRUE(queue.push(LongStr));
    for (auto i = 0; i < queueSize; ++i)
        ASSERT_FALSE(queue.push(ShortStr));
    for (auto i = 0; i < queueSize; ++i) {
        std::string str;
        ASSERT_TRUE(queue.pop(str));
        ASSERT_EQ(str, LongStr);
    }
}

TEST(SPSCQueue, RangePushPop)
{
    constexpr auto test = [](auto &queue, const std::size_t size) {
        const char ref = size % INT8_MAX;
        char tmp[size];
        for (auto &c : tmp)
            c = ref;
        ASSERT_TRUE(queue.tryPushRange(tmp, size));
        for (auto &c : tmp)
            c = 0;
        ASSERT_TRUE(queue.tryPopRange(tmp, size));
        for (const auto c : tmp)
            ASSERT_EQ(c, ref);
    };

    constexpr std::size_t maxQueueSize = 4096;

    for (auto queueSize = 1ul; queueSize < maxQueueSize; queueSize *= 2) {
        Core::SPSCQueue<char> queue(queueSize);
        ASSERT_FALSE(queue.tryPushRange(nullptr, queueSize + 1));
        ASSERT_FALSE(queue.tryPopRange(nullptr, 1));
        for (auto size = 1ul; size <= queueSize; ++size)
            test(queue, size);
        for (auto size = queueSize; size > 0; --size)
            test(queue, size);
    }

    Core::SPSCQueue<char> queue(maxQueueSize);
    char tmp[maxQueueSize * 2];
    for (auto &c : tmp)
        c = 42;
    ASSERT_EQ(queue.pushRange(tmp, maxQueueSize * 2), maxQueueSize);
    ASSERT_EQ(queue.popRange(tmp, maxQueueSize * 2), maxQueueSize);
}

TEST(SPSCQueue, InstensiveThreading)
{
    constexpr auto Counter = 10000;
    constexpr std::size_t queueSize = 4096;

    Core::SPSCQueue<int> queue(queueSize);

    std::thread thd([&queue] {
        for (auto i = 0; i < Counter; i += queue.push(i));
    });

    for (auto i = 0; i < Counter; ++i) {
        int tmp = 0;
        while (!queue.pop(tmp));
        ASSERT_EQ(tmp, i);
    }
    if (thd.joinable())
        thd.join();
}