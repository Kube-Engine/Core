/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark of SPMCQueue class
 */

#include <benchmark/benchmark.h>

#include <Kube/Core/SPMCQueue.hpp>

using namespace kF;

static void SPMCQueue_Append(benchmark::State &state)
{
    static Core::SPMCQueue<int> queue;
    static std::atomic<int> counter { 0 };

    if (state.thread_index == 0)
        counter = 0;

    for (auto _ : state) {
        queue.push(42);
    }

    ++counter;

    if (state.thread_index == 0) {
        while (counter.load() != state.threads);
        queue.clear();
    }
}

BENCHMARK(SPMCQueue_Append)->ThreadRange(1, 16);
