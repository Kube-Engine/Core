/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark of SPSCQueue class
 */

#include <thread>

#include <benchmark/benchmark.h>

#include <Kube/Core/SPSCQueue.hpp>

using namespace kF;

// More capacity means more memory space and less cache invalidation (can vary upon frequency of parrallel push / pop)
constexpr std::size_t Capacity = 4096 * 64;

static void SPSCQueuePush(benchmark::State &state)
{
    Core::SPSCQueue<int, Capacity> queue;

    int i = 0;
    for (auto _ : state) {
        if (queue.push(i))
            ++i;
        else // If we state.PauseTiming() it increase the benchmark score by 500ns which is too noisy
            queue.clear();
    }
}
BENCHMARK(SPSCQueuePush);

static void SPSCQueuePop(benchmark::State &state)
{
    Core::SPSCQueue<int, Capacity> queue;

    int i = 0;
    for (auto _ : state) {
        if (queue.pop(i))
            continue;
        else {
            state.PauseTiming();
            int j = 0;
            while (queue.push(++j));
            state.ResumeTiming();
        }
    }
}
BENCHMARK(SPSCQueuePop);

static void SPSCQueueNoisyPush(benchmark::State &state)
{
    std::atomic<bool> running = true;
    Core::SPSCQueue<int, Capacity> queue;

    // Thread to generate pop noise
    std::thread thd([&queue, &running] {
        int i = 0;
        while (running.load())
            while (queue.pop(i));
    });

    int i = 0;
    for (auto _ : state) {
        if (queue.push(i))
            ++i;
    }

    running.store(false);
    if (thd.joinable())
        thd.join();
}
BENCHMARK(SPSCQueueNoisyPush);

static void SPSCQueueNoisyPop(benchmark::State &state)
{
    std::atomic<bool> running = true;
    Core::SPSCQueue<int, Capacity> queue;

    // Thread to generate push noise
    std::thread thd([&queue, &running] {
        int i = 0;
        while (running.load()) {
            while (queue.push(i))
                ++i;
        }
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    int i = 0;
    for (auto _ : state) {
        if (!queue.pop(i))
            continue;
    }

    running.store(false);
    if (thd.joinable())
        thd.join();
}
BENCHMARK(SPSCQueueNoisyPop);