/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Benchmark of MPMCQueue class
 */

#include <thread>
#include <vector>

#include <benchmark/benchmark.h>

#include <Kube/Core/MPMCQueue.hpp>

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

#define GENERATE_TESTS_THREADS(TEST) \
    GENERATE_TESTS(TEST, 1, 1); \
    GENERATE_TESTS(TEST, 2, 2); \
    GENERATE_TESTS(TEST, 4, 4); \
    GENERATE_TESTS(TEST, 1, 2); \
    GENERATE_TESTS(TEST, 1, 4); \
    GENERATE_TESTS(TEST, 2, 1); \
    GENERATE_TESTS(TEST, 4, 1)

#define MPMCQUEUE_SINGLETHREADED_PUSH(Capacity, PackMembers) \
static void MPMCQueue_SingleThreadedPush_##Capacity##_##PackMembers(benchmark::State &state) \
{ \
    using Queue = Core::MPMCQueue<std::size_t, Capacity, PackMembers>; \
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
BENCHMARK(MPMCQueue_SingleThreadedPush_##Capacity##_##PackMembers)->UseManualTime();

GENERATE_TESTS(MPMCQUEUE_SINGLETHREADED_PUSH);

#define MPMCQUEUE_SINGLETHREADED_POP(Capacity, PackMembers) \
static void MPMCQueue_SingleThreadedPop_##Capacity##_##PackMembers(benchmark::State &state) \
{ \
    using Queue = Core::MPMCQueue<std::size_t, Capacity, PackMembers>; \
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
BENCHMARK(MPMCQueue_SingleThreadedPop_##Capacity##_##PackMembers)->UseManualTime();

GENERATE_TESTS(MPMCQUEUE_SINGLETHREADED_POP);

#define MPMCQUEUE_NOISY_PUSH(Capacity, PackMembers, PushCount, PopCount) \
static void MPMCQueue_NoisyPush_##Capacity##_##PackMembers##_##PushCount##_##PopCount(benchmark::State &state) \
{ \
    using Queue = Core::MPMCQueue<std::size_t, Capacity, PackMembers>; \
    Queue queue; \
    std::atomic<bool> running = true; \
    std::size_t i = 0ul; \
    std::vector<std::thread> pushThds; \
    std::vector<std::thread> popThds; \
    for (auto i = 0; i < PushCount - 1; ++i) pushThds.emplace_back([&queue, &running] { while (running) benchmark::DoNotOptimize(queue.push(42ul)); }); \
    for (auto i = 0; i < PopCount; ++i) pushThds.emplace_back([&queue, &running] { std::size_t tmp; while (running) benchmark::DoNotOptimize(queue.pop(tmp)); }); \
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
    for (auto &thd : pushThds) \
        if (thd.joinable()) \
            thd.join(); \
    for (auto &thd : popThds) \
        if (thd.joinable()) \
            thd.join(); \
} \
BENCHMARK(MPMCQueue_NoisyPush_##Capacity##_##PackMembers##_##PushCount##_##PopCount)->UseManualTime();

GENERATE_TESTS_THREADS(MPMCQUEUE_NOISY_PUSH);

#define MPMCQUEUE_NOISY_POP(Capacity, PackMembers, PushCount, PopCount) \
static void MPMCQueue_NoisyPop_##Capacity##_##PackMembers##_##PushCount##_##PopCount(benchmark::State &state) \
{ \
    using Queue = Core::MPMCQueue<std::size_t, Capacity, PackMembers>; \
    Queue queue; \
    std::atomic<bool> running = true; \
    std::size_t i = 0ul; \
    std::vector<std::thread> pushThds; \
    std::vector<std::thread> popThds; \
    for (auto i = 0; i < PushCount; ++i) pushThds.emplace_back([&queue, &running] { while (running) benchmark::DoNotOptimize(queue.push(42ul)); }); \
    for (auto i = 0; i < PopCount - 1; ++i) pushThds.emplace_back([&queue, &running] { std::size_t tmp; while (running) benchmark::DoNotOptimize(queue.pop(tmp)); }); \
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
    for (auto &thd : pushThds) \
        if (thd.joinable()) \
            thd.join(); \
    for (auto &thd : popThds) \
        if (thd.joinable()) \
            thd.join(); \
} \
BENCHMARK(MPMCQueue_NoisyPop_##Capacity##_##PackMembers##_##PushCount##_##PopCount)->UseManualTime();

GENERATE_TESTS_THREADS(MPMCQUEUE_NOISY_POP);
