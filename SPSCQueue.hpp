/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SPSC Queue
 */

#pragma once

#include <atomic>
#include <cstdlib>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type>
    class SPSCQueue;
}

/**
 * @brief The SPSC queue is a lock-free queue that only supports a Single Producer and a Single Consumer
 * The queue is really fast compared to other more flexible implementations because the fact that only two thread can simultaneously read / write
 * means that less synchronization is needed for each operation.
 * The queue supports ranged push / pop to insert multiple elements without performance impact
 *
 * @tparam Type to be inserted
 */
template<typename Type>
class KF_ALIGN_CACHELINE2 kF::Core::SPSCQueue
{
public:
    /** @brief Buffer structure containing all cells */
    struct Buffer
    {
        Type *data { nullptr };
        std::size_t capacity { 0 };
    };

    /** @brief Local thread cache */
    struct Cache
    {
        Buffer buffer {};
        std::size_t value { 0 };
    };

    /** @brief Default constructor initialize the queue
     * If 'usedAsBuffer' is true, capacity will be increased by 1 because the queue implementation needs one unused value when the queue is full
    */
    SPSCQueue(const std::size_t capacity, const bool usedAsBuffer = true);

    /** @brief Destruct and release all memory (unsafe) */
    ~SPSCQueue(void) noexcept_destructible(Type);

    /** @brief Push a single element into the queue
     *  @return true if the element has been inserted */
    template<typename ...Args>
    [[nodiscard]] bool push(Args &&...args) noexcept_constructible(Type, Args...);

    /** @brief Pop a single element from the queue
     *  @return true if an element has been extracted */
    [[nodiscard]] bool pop(Type &value) noexcept(nothrow_destructible(Type) && (std::is_move_assignable_v<Type> ? nothrow_move_assignable(Type) : nothrow_move_constructible(Type)));

    /** @brief Push exactly 'count' elements into the queue
     *  @tparam ForceCopy If true, will prevent to move construct elements
     *  @return Success on true */
    template<bool ForceCopy = false>
    [[nodiscard]] bool tryPushRange(Type *data, const std::size_t count) noexcept(!ForceCopy && std::is_move_assignable_v<Type> ? nothrow_move_assignable(Type) : nothrow_move_constructible(Type)) { return pushRangeImpl<false, ForceCopy>(data, count); }

    /** @brief Pop exactly 'count' elements from the queue
     *  @return Success on true */
    [[nodiscard]] bool tryPopRange(Type *data, const std::size_t count) noexcept(nothrow_destructible(Type) && (std::is_move_assignable_v<Type> ? nothrow_move_assignable(Type) : nothrow_move_constructible(Type))) { return popRangeImpl<false>(data, count); }

    /** @brief Push up to 'count' elements into the queue
     *  @tparam ForceCopy If true, will prevent to move construct elements
     *  @return The number of extracted elements */
    template<bool ForceCopy = false>
    [[nodiscard]] std::size_t pushRange(Type *data, const std::size_t count) noexcept(!ForceCopy && std::is_move_assignable_v<Type> ? nothrow_move_assignable(Type) : nothrow_move_constructible(Type)) { return pushRangeImpl<true, ForceCopy>(data, count); }

    /** @brief Pop up to 'count' elements from the queue
     *  @return The number of extracted elements */
    [[nodiscard]] std::size_t popRange(Type *data, const std::size_t count) noexcept(nothrow_destructible(Type) && (std::is_move_assignable_v<Type> ? nothrow_move_assignable(Type) : nothrow_move_constructible(Type))) { return popRangeImpl<true>(data, count); }

    /** @brief Clear all elements of the queue (unsafe) */
    void clear(void) noexcept_destructible(Type);

private:
    KF_ALIGN_CACHELINE std::atomic<size_t> _tail { 0 }; // Tail accessed by both producer and consumer
    KF_ALIGN_CACHELINE Cache _tailCache {}; // Cache accessed by consumer thread

    KF_ALIGN_CACHELINE std::atomic<size_t> _head { 0 }; // Head accessed by both producer and consumer
    KF_ALIGN_CACHELINE Cache _headCache {}; // Cache accessed by producer thread

    /** @brief Copy and move constructors disabled */
    SPSCQueue(const SPSCQueue &other) = delete;
    SPSCQueue(SPSCQueue &&other) = delete;

    template<bool AllowLess, bool ForceCopy>
    [[nodiscard]] std::size_t pushRangeImpl(Type *data, const std::size_t count) noexcept(!ForceCopy && std::is_move_assignable_v<Type> ? nothrow_move_assignable(Type) : nothrow_move_constructible(Type));

    template<bool AllowLess>
    [[nodiscard]] std::size_t popRangeImpl(Type *data, const std::size_t count) noexcept(nothrow_destructible(Type) && (std::is_move_assignable_v<Type> ? nothrow_move_assignable(Type) : nothrow_move_constructible(Type)));
};

static_assert(sizeof(kF::Core::SPSCQueue<int>) == 4 * kF::Core::Utils::CacheLineSize);

#include "SPSCQueue.ipp"