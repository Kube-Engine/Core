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
    template<typename Type, std::size_t Capacity, bool PackMembers>
    class SPSCQueue;
}

/**
 * @brief The SPSC queue is a lock-free queue that only supports a Single Producer and a Single Consumer
 * The queue is really fast compared to other implementations because the fact that only two thread can simultaneously read / write
 * means that less synchronization is needed for each operation.
 * The queue supports ranged push / pop to insert multiple elements without performance impact
 *
 * @tparam Type to be inserted
 * @tparam Capacity of the ring buffer
 */
template<typename Type, std::size_t Capacity, bool PackMembers = false>
class kF::Core::SPSCQueue
{
public:
    static_assert(Capacity > 1, "A queue must have at least a capacity of 2");

    /** @brief Default constructor initialize the queue */
    SPSCQueue(void) noexcept : _buffer(reinterpret_cast<Type *>(std::malloc(sizeof(Type) * Capacity))) {}

    /** @brief Copy and move constructors disabled */
    SPSCQueue(const SPSCQueue &other) = delete;
    SPSCQueue(SPSCQueue &&other) = delete;

    /** @brief Destruct and release all memory (unsafe) */
    ~SPSCQueue(void) noexcept(std::is_nothrow_destructible_v<Type>) { clear(); std::free(_buffer); }

    /** @brief Get queue's size */
    [[nodiscard]] std::size_t size(void) const noexcept { return _size.load(); }

    /** @brief Push a single element into the queue
     *  @return true if the element has been inserted */
    template<typename ...Args>
    [[nodiscard]] bool push(Args &&...args) noexcept(std::is_nothrow_constructible_v<Type, Args...>);

    /** @brief Pop a single element from the queue
     *  @return true if an element has been extracted */
    [[nodiscard]] bool pop(Type &value) noexcept(Utils::NothrowCopyOrMoveAssign<Type>);

    /** @brief Push multiple elements into the queue
     *  @tparam ForceCopy If true, will prevent to move construct elements
     *  @return The number inserted elements */
    template<bool ForceCopy = false>
    [[nodiscard]] std::size_t pushRange(Type *data, const std::size_t max) noexcept(Utils::NothrowCopyOrMoveConstruct<Type, ForceCopy>);

    /** @brief Pop up to 'max' elements from the queue
     *  @return The number of extracted elements */
    [[nodiscard]] std::size_t popRange(Type *data, const std::size_t max) noexcept(Utils::NothrowCopyOrMoveAssign<Type>);

    /** @brief Clear all elements of the queue (unsafe) */
    void clear(void) noexcept(std::is_nothrow_destructible_v<Type>);

private:
    KF_CACHELINE_ALIGNED_MEMBER_IF(!PackMembers, std::atomic<std::size_t>, _size, 0); // Size accessed by both producer and consumer
    KF_CACHELINE_ALIGNED_MEMBER_IF(!PackMembers, std::size_t, _head, 0); // Head only accesses by consumer
    KF_CACHELINE_ALIGNED_MEMBER_IF(!PackMembers, std::size_t, _tail, 0); // Tail only accesses by producer
    KF_CACHELINE_ALIGNED_MEMBER_IF(!PackMembers, Type *, _buffer, nullptr); // Buffer accessed by both producer and consumers

    /** @brief Get the next index of value */
    [[nodiscard]] static std::size_t Next(const std::size_t value) noexcept { return (value + 1) % Capacity; }

    /** @brief Increment value by count */
    [[nodiscard]] static std::size_t Increment(const std::size_t value, const std::size_t count) noexcept { return (value + count) % Capacity; }
};

static_assert(sizeof(kF::Core::SPSCQueue<int, 4096, true>) == (sizeof(std::atomic<std::size_t>) + sizeof(std::size_t) * 2 + sizeof(int *)));
static_assert(sizeof(kF::Core::SPSCQueue<int, 4096, false>) == (kF::Core::Utils::CacheLineSize * 4));

#include "SPSCQueue.ipp"