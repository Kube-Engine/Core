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
    template<typename Type, std::size_t Capacity, bool PackMembers>
    class MPMCQueue;
}

/**
 * @brief The MPMC queue is a lock-free queue that only supports a Single Producer but Multiple Consumers
 *
 * @tparam Type to be inserted
 * @tparam Capacity of the ring buffer
 */
template<typename Type, std::size_t Capacity, bool PackMembers = false>
class kF::Core::MPMCQueue
{
public:
    static_assert(Capacity > 1, "A queue must have at least a capacity of 2");

    struct State
    {
        KF_CACHELINE_ALIGNED_MEMBER_IF(!PackMembers, std::atomic<std::size_t>, sequence, 0);
    };

    /** @brief Default constructor initialize the queue */
    MPMCQueue(void) noexcept;

    /** @brief Copy and move constructors disabled */
    MPMCQueue(const MPMCQueue &other) = delete;
    MPMCQueue(MPMCQueue &&other) = delete;

    /** @brief Destruct and release all memory (unsafe) */
    ~MPMCQueue(void) noexcept(std::is_nothrow_destructible_v<Type>) { clear(); std::free(_buffer); std::free(_states); }

    /** @brief Get queue's size */
    [[nodiscard]] std::size_t size(void) const noexcept { return Distance(_head.load(), _tail.load()); }

    /** @brief Push a single element into the queue
     *  @return true if the element has been inserted */
    template<typename ...Args>
    [[nodiscard]] bool push(Args &&...args) noexcept(std::is_nothrow_constructible_v<Type, Args...>);

    /** @brief Pop a single element from the queue
     *  @return true if an element has been extracted */
    [[nodiscard]] bool pop(Type &value) noexcept(Utils::NothrowCopyOrMoveAssign<Type>);

    /** @brief Clear all elements of the queue (unsafe) */
    void clear(void) noexcept(Utils::NothrowCopyOrMoveAssign<Type>) { for (Type tmp; pop(tmp);); }

private:
    KF_CACHELINE_ALIGNED_MEMBER_IF(!PackMembers, std::atomic<std::size_t>, _head, 0); // Head accessed by consumers
    KF_CACHELINE_ALIGNED_MEMBER_IF(!PackMembers, std::atomic<std::size_t>, _tail, 0); // Tail accessed by producers
    KF_CACHELINE_ALIGNED_MEMBER_IF(!PackMembers, Type *, _buffer, 0); // Buffer accessed by both producer and consumers
    KF_CACHELINE_ALIGNED_MEMBER_IF(!PackMembers, State *, _states, 0); // State of inserted elements accessed by both producer and consumers

    /** @brief Get the next index of value */
    [[nodiscard]] static std::size_t Distance(const std::size_t x, const std::size_t y) noexcept { return x > y ? x - y : y - x; }
};

static_assert(sizeof(kF::Core::MPMCQueue<int, 4096, true>) == sizeof(std::atomic<std::size_t>) * 2 + sizeof(int *) * 2);
static_assert(sizeof(kF::Core::MPMCQueue<int, 4096, false>) == kF::Core::Utils::CacheLineSize * 4);

#include "MPMCQueue.ipp"