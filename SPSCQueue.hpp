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
 * The queue is really fast compared to other implementations because the fact that only two thread can simultaneously read / write
 * means that less synchronization is needed for each operation.
 * The queue supports ranged push / pop to insert multiple elements without performance impact
 *
 * @tparam Type to be inserted
 */
template<typename Type>
class kF::Core::SPSCQueue
{
public:
    /** @brief Buffer structure containing all cells */
    struct Buffer
    {
        Type *data { nullptr };
        std::size_t capacity { 0 };
    };

    /** @brief Default constructor initialize the queue */
    SPSCQueue(const std::size_t capacity);

    /** @brief Destruct and release all memory (unsafe) */
    ~SPSCQueue(void) noexcept(std::is_nothrow_destructible_v<Type>) { clear(); std::free(_buffer.data); }

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
    KF_ALIGN_CACHELINE2 Buffer _buffer;

    KF_ALIGN_CACHELINE2 std::atomic<size_t> _tail { 0 }; // Tail accessed by both producer and consumer
    std::size_t _headCache { 0 }; // Head cache, accessed only by producer

    KF_ALIGN_CACHELINE2 std::atomic<size_t> _head { 0 }; // Head accessed by both producer and consumer
    std::size_t _tailCache { 0 }; // Tail cache, accessed only by consumer

    /** @brief Copy and move constructors disabled */
    SPSCQueue(const SPSCQueue &other) = delete;
    SPSCQueue(SPSCQueue &&other) = delete;
};

#include "SPSCQueue.ipp"