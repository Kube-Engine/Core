/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MPMC Queue
 */

#pragma once

#include <atomic>
#include <cstdlib>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type>
    class MPMCQueue;
}

/**
 * @brief The MPMC queue is a lock-free queue that only supports a Single Producer but Multiple Consumers
 *
 * @tparam Type to be inserted
 */
template<typename Type>
class KF_ALIGN_CACHELINE2 kF::Core::MPMCQueue
{
public:
    /** @brief Each cell represent the queued type and a sequence index */
    struct Cell
    {
        std::atomic<std::size_t> sequence { 0 };
        Type data;
    };

    /** @brief Buffer structure containing all cells */
    struct Buffer
    {
        std::size_t mask { 0 };
        Cell *data { nullptr };
    };

    /** @brief Cache of producers or consumers */
    struct Cache
    {
        Buffer buffer;
    };

    /** @brief Default constructor initialize the queue */
    MPMCQueue(const std::size_t size);

    /** @brief Destruct and release all memory (unsafe) */
    ~MPMCQueue(void) noexcept(std::is_nothrow_destructible_v<Type>);

    /** @brief Push a single element into the queue
     *  @return true if the element has been inserted */
    template<bool MoveOnSuccess = false, typename ...Args>
    [[nodiscard]] inline bool push(Args &&...args) noexcept(std::is_nothrow_constructible_v<Type, Args...>);

    /** @brief Pop a single element from the queue
     *  @return true if an element has been extracted */
    [[nodiscard]] inline bool pop(Type &value) noexcept(Utils::NothrowCopyOrMoveAssign<Type>);

    /** @brief Clear all elements of the queue (unsafe) */
    void clear(void) noexcept(Utils::NothrowCopyOrMoveAssign<Type>) { for (Type tmp; pop(tmp);); }

private:
    KF_ALIGN_CACHELINE std::atomic<std::size_t> _tail { 0 }; // Tail accessed by producers
    KF_ALIGN_CACHELINE Cache _tailCache; // Cache accessed by producers
    KF_ALIGN_CACHELINE std::atomic<std::size_t> _head { 0 }; // Head accessed by consumers
    KF_ALIGN_CACHELINE Cache _headCache; // Cache accessed by consumers

    /** @brief Copy and move constructors disabled */
    MPMCQueue(const MPMCQueue &other) = delete;
    MPMCQueue(MPMCQueue &&other) = delete;
};

static_assert(sizeof(kF::Core::MPMCQueue<int>) == 4 * kF::Core::Utils::CacheLineSize);

#include "MPMCQueue.ipp"