/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark of SafeQueue class
 */

#include <benchmark/benchmark.h>

#include <Kube/Core/SafeQueue.hpp>

using namespace kF;

static void Vector_Append(benchmark::State &state)
{
    std::vector<int> queue;

    for (auto _ : state) {
        queue.emplace_back(42);
    }
}
BENCHMARK(Vector_Append);

static void SafeQueue_Append(benchmark::State &state)
{
    static std::atomic<int> count { 0 };
    static Core::SafeQueue<int> queue;
    auto producer = queue.acquireProducer();

    for (auto _ : state) {
        producer.data().emplace_back(42);
    }
    producer.release();
    ++count;
    if (state.thread_index == 0) {
        while (count != state.threads);
        count = 0;
        queue.releaseAllMemory();
    }
}

BENCHMARK(SafeQueue_Append)->ThreadRange(1, 16);

static void Vector_AppendRange(benchmark::State &state)
{
    std::vector<int> queue;

    for (auto _ : state) {
        auto max = state.range(0);
        for (auto i = 0; i < max; ++i)
            queue.emplace_back(42);
        queue.clear();
        queue.shrink_to_fit();
    }
}
BENCHMARK(Vector_AppendRange)->Range(1, 8*8*8*8*8)->RangeMultiplier(4);

static void SafeQueue_AppendRange(benchmark::State &state)
{
    static std::atomic<int> count { 0 };
    static Core::SafeQueue<int> queue;

    for (auto _ : state) {
        auto producer = queue.acquireProducer();
        auto max = state.range(0);
        for (auto i = 0l; i < max; ++i)
            producer.data().emplace_back(42);
        // producer.data().clear();
        // producer.data().shrink_to_fit();
        // producer.release();
    }
    ++count;
    if (state.thread_index == 0) {
        while (count != state.threads);
        count = 0;
        queue.releaseAllMemory();
    }
}

BENCHMARK(SafeQueue_AppendRange)->Range(1, 8*8*8*8*8)->RangeMultiplier(4)->ThreadRange(1, 16);
