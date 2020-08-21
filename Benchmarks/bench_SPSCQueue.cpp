/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark of SPSCQueue class
 */

#include <thread>

#include <benchmark/benchmark.h>

#include <Kube/Core/SPSCQueue.hpp>

using namespace kF;

#define GENERATE_TESTS(TEST, ...) \
    TEST(8, true __VA_OPT__(,) __VA_ARGS__); \
    TEST(8, false __VA_OPT__(,) __VA_ARGS__); \
    TEST(64, true __VA_OPT__(,) __VA_ARGS__); \
    TEST(64, false __VA_OPT__(,) __VA_ARGS__); \
    TEST(512, true __VA_OPT__(,) __VA_ARGS__); \
    TEST(512, false __VA_OPT__(,) __VA_ARGS__); \
    TEST(4096, true __VA_OPT__(,) __VA_ARGS__); \
    TEST(4096, false __VA_OPT__(,) __VA_ARGS__); \
    TEST(32768, true __VA_OPT__(,) __VA_ARGS__); \
    TEST(32768, false __VA_OPT__(,) __VA_ARGS__)

#define SPSCQUEUE_SINGLETHREADED_PUSH(Capacity, PackMembers) \
static void SPSCQueue_SingleThreadedPush_##Capacity##_##PackMembers(benchmark::State &state) \
{ \
    using Queue = Core::SPSCQueue<std::size_t, Capacity, PackMembers>; \
    Queue queue; \
    std::size_t i = 0ul; \
    for (auto _ : state) { \
        queue.clear(); \
        auto start = std::chrono::high_resolution_clock::now(); \
        for (auto j = 0ul; j < Capacity; ++j) \
            benchmark::DoNotOptimize(queue.push(42ul)); \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start); \
        auto iterationTime = elapsed.count(); \
        state.SetIterationTime(iterationTime); \
    } \
} \
BENCHMARK(SPSCQueue_SingleThreadedPush_##Capacity##_##PackMembers)->UseManualTime();

GENERATE_TESTS(SPSCQUEUE_SINGLETHREADED_PUSH);

#define SPSCQUEUE_SINGLETHREADED_POP(Capacity, PackMembers) \
static void SPSCQueue_SingleThreadedPop_##Capacity##_##PackMembers(benchmark::State &state) \
{ \
    using Queue = Core::SPSCQueue<std::size_t, Capacity, PackMembers>; \
    Queue queue; \
    std::size_t i = 0ul; \
    for (auto _ : state) { \
        while (queue.push(42ul)); \
        std::size_t tmp; \
        auto start = std::chrono::high_resolution_clock::now(); \
        while (queue.pop(tmp)); \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start); \
        auto iterationTime = elapsed.count(); \
        state.SetIterationTime(iterationTime); \
    } \
} \
BENCHMARK(SPSCQueue_SingleThreadedPop_##Capacity##_##PackMembers)->UseManualTime();

GENERATE_TESTS(SPSCQUEUE_SINGLETHREADED_POP);

#define SPSCQUEUE_NOISY_PUSH(Capacity, PackMembers) \
static void SPSCQueue_NoisyPush_##Capacity##_##PackMembers(benchmark::State &state) \
{ \
    using Queue = Core::SPSCQueue<std::size_t, Capacity, PackMembers>; \
    Queue queue; \
    std::atomic<bool> running = true; \
    std::size_t i = 0ul; \
    std::thread thd([&queue, &running] { for (std::size_t tmp; running; benchmark::DoNotOptimize(queue.pop(tmp))); }); \
    for (auto _ : state) { \
        decltype(std::chrono::high_resolution_clock::now()) start; \
        do { \
            start = std::chrono::high_resolution_clock::now(); \
        } while (!queue.push(42ul)); \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start); \
        auto iterationTime = elapsed.count(); \
        state.SetIterationTime(iterationTime); \
    } \
    running = false; \
    if (thd.joinable()) \
        thd.join(); \
} \
BENCHMARK(SPSCQueue_NoisyPush_##Capacity##_##PackMembers)->UseManualTime();

GENERATE_TESTS(SPSCQUEUE_NOISY_PUSH);

#define SPSCQUEUE_NOISY_POP(Capacity, PackMembers) \
static void SPSCQueue_NoisyPop_##Capacity##_##PackMembers(benchmark::State &state) \
{ \
    using Queue = Core::SPSCQueue<std::size_t, Capacity, PackMembers>; \
    Queue queue; \
    std::atomic<bool> running = true; \
    std::size_t i = 0ul; \
    std::thread thd([&queue, &running] { while (running) benchmark::DoNotOptimize(queue.push(42ul)); }); \
    for (auto _ : state) { \
        std::size_t tmp; \
        decltype(std::chrono::high_resolution_clock::now()) start; \
        do { \
            start = std::chrono::high_resolution_clock::now(); \
        } while (!queue.pop(tmp)); \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start); \
        auto iterationTime = elapsed.count(); \
        state.SetIterationTime(iterationTime); \
    } \
    running = false; \
    if (thd.joinable()) \
        thd.join(); \
} \
BENCHMARK(SPSCQueue_NoisyPop_##Capacity##_##PackMembers)->UseManualTime();

GENERATE_TESTS(SPSCQUEUE_NOISY_POP);