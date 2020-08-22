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
    constexpr std::size_t queueSize = 8;

    Core::SPSCQueue<std::string> queue(queueSize);
    std::vector<std::string> data(queueSize, LongStr), data2(queueSize, ShortStr), tmp(queueSize);

    ASSERT_EQ(queue.pushRange<true>(data.data(), data.size() * 2), queueSize);
    ASSERT_EQ(queue.pushRange<true>(data2.data(), data2.size()), 0);
    ASSERT_EQ(queue.popRange(tmp.data(), tmp.size()), queueSize);
    for (auto &str : tmp)
        ASSERT_EQ(str, LongStr);

    ASSERT_EQ(queue.pushRange(data2.data(), queueSize - 1), queueSize - 1);
    ASSERT_EQ(queue.pushRange(data.data(), data.size()), 1);
    ASSERT_EQ(queue.popRange(tmp.data(), tmp.size()), queueSize);
    for (auto i = 0u; auto &str : tmp) {
        if (i + 1 == tmp.size())
            ASSERT_EQ(str, LongStr);
        else
            ASSERT_EQ(str, ShortStr);
        ++i;
    }
    ASSERT_EQ(data[0], "");
    for (auto i = 1; i < queueSize; ++i)
        ASSERT_EQ(data[i], LongStr);
    for (auto i = 0; i < queueSize - 1; ++i)
        ASSERT_EQ(data2[i], "");
    ASSERT_EQ(data2[queueSize - 1], ShortStr);
}

TEST(SPSCQueue, InstensiveThreading)
{
    constexpr auto Counter = 10000000;
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