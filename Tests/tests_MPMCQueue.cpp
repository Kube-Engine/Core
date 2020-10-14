/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Tests of the MPMC Queue
 */

#include <thread>

#include <gtest/gtest.h>

#include <Kube/Core/Assert.hpp>
#include <Kube/Core/MPMCQueue.hpp>

using namespace kF;

constexpr auto LongStr = "123456789123456789";
constexpr auto ShortStr = "12345";

TEST(MPMCQueue, SinglePushPop)
{
    constexpr std::size_t queueSize = 8;

    Core::MPMCQueue<std::string> queue(queueSize);

    for (auto i = 0u; i < queueSize; ++i)
        ASSERT_TRUE(queue.push(LongStr));
    for (auto i = 0u; i < queueSize; ++i)
        ASSERT_FALSE(queue.push(ShortStr));
    for (auto i = 0u; i < queueSize; ++i) {
        std::string str;
        ASSERT_TRUE(queue.pop(str));
        ASSERT_EQ(str, LongStr);
    }
}

TEST(MPMCQueue, InstensiveThreading)
{
    constexpr auto ThreadCount = 4;
    constexpr auto Counter = KUBE_DEBUG_BUILD ? 100 : 10000;
    constexpr std::size_t queueSize = 4096;

    static std::atomic<bool> running { true };
    static std::atomic<std::size_t> pushingThds { 0 };
    static std::atomic<std::size_t> popCount { 0 };

    std::thread pushThds[ThreadCount];
    std::thread popThds[ThreadCount];

    Core::MPMCQueue<int> queue(queueSize);

    for (auto i = 0; i < ThreadCount; ++i)
        pushThds[i] = std::thread([&queue] {
            ++pushingThds;
            for (auto i = 0; i < Counter / ThreadCount;)
                i += queue.push(i);
            --pushingThds;
        });
    for (auto i = 0; i < ThreadCount; ++i)
        popThds[i] = std::thread([&queue] {
            while (running) {
                int tmp = 0;
                while (queue.pop(tmp))
                    ++popCount;
            }
        });
    while (pushingThds)
        std::this_thread::yield();
    for (auto i = 0; i < ThreadCount; ++i) {
        if (pushThds[i].joinable())
            pushThds[i].join();
    }
    while (popCount != Counter)
        std::this_thread::yield();
    running = false;
    for (auto i = 0; i < ThreadCount; ++i) {
        if (popThds[i].joinable())
            popThds[i].join();
    }
}