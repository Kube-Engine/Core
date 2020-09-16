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
    ~SPSCQueue(void) noexcept(std::is_nothrow_destructible_v<Type>);

    /** @brief Push a single element into the queue
     *  @return true if the element has been inserted */
    template<typename ...Args>
    [[nodiscard]] inline bool push(Args &&...args) noexcept(std::is_nothrow_constructible_v<Type, Args...>);

    /** @brief Pop a single element from the queue
     *  @return true if an element has been extracted */
    [[nodiscard]] inline bool pop(Type &value) noexcept(Utils::NothrowCopyOrMoveAssign<Type>);

    /** @brief Push multiple elements into the queue
     *  @tparam ForceCopy If true, will prevent to move construct elements
     *  @return Success on true */
    template<bool ForceCopy = false>
    [[nodiscard]] inline bool pushRange(Type *data, const std::size_t max) noexcept(Utils::NothrowCopyOrMoveConstruct<Type, ForceCopy>);

    /** @brief Pop up to 'max' elements from the queue
     *  @return Success on true */
    [[nodiscard]] inline bool popRange(Type *data, const std::size_t max) noexcept(Utils::NothrowCopyOrMoveAssign<Type>);

    /** @brief Clear all elements of the queue (unsafe) */
    void clear(void) noexcept(std::is_nothrow_destructible_v<Type>);

public:
    KF_ALIGN_CACHELINE std::atomic<size_t> _tail { 0 }; // Tail accessed by both producer and consumer
    KF_ALIGN_CACHELINE Cache _tailCache;

    KF_ALIGN_CACHELINE std::atomic<size_t> _head { 0 }; // Head accessed by both producer and consumer
    KF_ALIGN_CACHELINE Cache _headCache;

    /** @brief Copy and move constructors disabled */
    SPSCQueue(const SPSCQueue &other) = delete;
    SPSCQueue(SPSCQueue &&other) = delete;
};

static_assert(sizeof(kF::Core::SPSCQueue<int>) == 4 * kF::Core::Utils::CacheLineSize);

static_assert(offsetof(kF::Core::SPSCQueue<int>, _tail) == 0);
static_assert(offsetof(kF::Core::SPSCQueue<int>, _tailCache) == 64);
static_assert(offsetof(kF::Core::SPSCQueue<int>, _head) == 128);
static_assert(offsetof(kF::Core::SPSCQueue<int>, _headCache) == 192);

#include "SPSCQueue.ipp"