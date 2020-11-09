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
class alignas_double_cacheline kF::Core::SPSCQueue
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
     * If 'usedAsBuffer' is true, capacity will be increased by 1 because the queue implementation needs one unused value when the queue is full */
    SPSCQueue(const std::size_t capacity, const bool usedAsBuffer = true) noexcept;

    /** @brief Destruct and release all memory (unsafe) */
    ~SPSCQueue(void) noexcept_destructible(Type);

    /** @brief Push a single element into the queue
     *  @return true if the element has been inserted */
    template<typename ...Args>
    [[nodiscard]] bool push(Args &&...args)
        noexcept_constructible(Type, Args...)
        requires std::constructible_from<Type, Args...>;

    /** @brief Pop a single element from the queue
     *  @return true if an element has been extracted */
    [[nodiscard]] bool pop(Type &value)
        noexcept(nothrow_destructible(Type) && nothrow_forward_assignable(Type));

    /** @brief Push exactly 'count' elements into the queue
     *  @tparam ForceCopy If true, will prevent to move construct elements
     *  @return Success on true */
    template<std::input_iterator InputIterator>
    [[nodiscard]] bool tryPushRange(const InputIterator from, const InputIterator to)
        noexcept_forward_constructible(Type)
        { return pushRangeImpl<false>(from, to); }

    /** @brief Pop exactly 'count' elements from the queue
     *  @return Success on true */
    template<typename OutputIterator> requires std::output_iterator<OutputIterator, Type>
    [[nodiscard]] bool tryPopRange(const OutputIterator from, const OutputIterator to)
        noexcept(nothrow_destructible(Type) && nothrow_forward_assignable(Type))
        { return popRangeImpl<false>(from, to); }

    /** @brief Push up to 'count' elements into the queue
     *  @tparam ForceCopy If true, will prevent to move construct elements
     *  @return The number of extracted elements */
    template<std::input_iterator InputIterator>
    [[nodiscard]] std::size_t pushRange(const InputIterator from, const InputIterator to)
        noexcept_forward_assignable(Type)
        { return pushRangeImpl<true>(from, to); }

    /** @brief Pop up to 'count' elements from the queue
     *  @return The number of extracted elements */
    template<typename OutputIterator> requires std::output_iterator<OutputIterator, Type>
    [[nodiscard]] std::size_t popRange(const OutputIterator from, const OutputIterator to)
        noexcept(nothrow_destructible(Type) && nothrow_forward_assignable(Type))
        { return popRangeImpl<true>(from, to); }

    /** @brief Clear all elements of the queue (unsafe) */
    void clear(void) noexcept_destructible(Type);

    /** @brief Get the size of the queue */
    [[nodiscard]] std::size_t size(void) const noexcept;

private:
    alignas_cacheline std::atomic<size_t> _tail { 0 }; // Tail accessed by both producer and consumer
    alignas_cacheline Cache _tailCache {}; // Cache accessed by consumer thread

    alignas_cacheline std::atomic<size_t> _head { 0 }; // Head accessed by both producer and consumer
    alignas_cacheline Cache _headCache {}; // Cache accessed by producer thread

    /** @brief Copy and move constructors disabled */
    SPSCQueue(const SPSCQueue &other) = delete;
    SPSCQueue(SPSCQueue &&other) = delete;

    template<bool AllowLess, std::input_iterator InputIterator>
    [[nodiscard]] std::size_t pushRangeImpl(const InputIterator from, const InputIterator to)
        noexcept(nothrow_forward_iterator_constructible(InputIterator));

    template<bool AllowLess, typename OutputIterator> requires std::output_iterator<OutputIterator, Type>
    [[nodiscard]] std::size_t popRangeImpl(const OutputIterator from, const OutputIterator to)
        noexcept(nothrow_destructible(Type) && nothrow_forward_assignable(Type));
};

static_assert_sizeof(kF::Core::SPSCQueue<int>, 4 * kF::Core::CacheLineSize);
static_assert_alignof_double_cacheline(kF::Core::SPSCQueue<int>);

#include "SPSCQueue.ipp"