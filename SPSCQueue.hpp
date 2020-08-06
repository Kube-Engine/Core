/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SPSC Queue
 */

#pragma once

#include <atomic>

namespace kF::Core
{
    template<typename Type, std::size_t Capacity>
    class SPSCQueue;

    namespace SPSCQueueUtils
    {
        /** @brief Helper used to use the right noexcept mode uppon forwarding constructor */
        template<typename Type, bool ForceCopy = false>
        constexpr bool NothrowCopyOrMoveConstruct = [] {
            if constexpr (!ForceCopy && std::is_move_constructible_v<Type>)
                return std::is_nothrow_move_constructible_v<Type>;
            else
                return std::is_nothrow_copy_constructible_v<Type>;
        }();

        /** @brief Helper used to use the right noexcept mode uppon forwarding assignment */
        template<typename Type, bool ForceCopy = false>
        constexpr bool NothrowCopyOrMoveAssign = [] {
            if constexpr (!ForceCopy && std::is_move_assignable_v<Type>)
                return std::is_nothrow_move_assignable_v<Type>;
            else
                return std::is_nothrow_copy_assignable_v<Type>;
        }();
    }
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
template<typename Type, std::size_t Capacity>
class kF::Core::SPSCQueue
{
public:
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
    [[nodiscard]] bool pop(Type &value) noexcept(SPSCQueueUtils::NothrowCopyOrMoveAssign<Type>);

    /** @brief Push multiple elements into the queue
     *  @tparam ForceCopy If true, will prevent to move construct elements
     *  @return The number inserted elements */
    template<bool ForceCopy = false>
    [[nodiscard]] std::size_t pushRange(Type *data, const std::size_t max) noexcept(SPSCQueueUtils::NothrowCopyOrMoveConstruct<Type, ForceCopy>);

    /** @brief Pop up to 'max' elements from the queue
     *  @return The number of extracted elements */
    [[nodiscard]] std::size_t popRange(Type *data, const std::size_t max) noexcept(SPSCQueueUtils::NothrowCopyOrMoveAssign<Type>);

    /** @brief Clear all elements of the queue (unsafe) */
    void clear(void) noexcept(std::is_nothrow_destructible_v<Type>);

private:
    std::atomic<std::size_t> _size { 0 }; // Size accessed by both producer and consumer
    std::size_t _head { 0 }; // Head only accesses by consumer
    std::size_t _tail { 0 }; // Tail only accesses by producer
    Type *_buffer { nullptr };

    /** @brief Get the next index of value */
    [[nodiscard]] static std::size_t Next(const std::size_t value) noexcept { return (value + 1) % Capacity; }

    /** @brief Increment value by count */
    [[nodiscard]] static std::size_t Increment(const std::size_t value, const std::size_t count) noexcept { return (value + count) % Capacity; }

    /** @brief Helper used to perfect forward move / copy constructor */
    template<bool ForceCopy = false>
    static void ForwardConstruct(Type *dest, Type *source) noexcept(SPSCQueueUtils::NothrowCopyOrMoveConstruct<Type>) {
        if constexpr (!ForceCopy && std::is_move_assignable_v<Type>)
            new (dest) Type(std::move(*source));
        else
            new (dest) Type(*source);
    }

    /** @brief Helper used to perfect forward move / copy assignment */
    template<bool ForceCopy = false>
    static void ForwardAssign(Type *dest, Type *source) noexcept(SPSCQueueUtils::NothrowCopyOrMoveAssign<Type>) {
        if constexpr (!ForceCopy && std::is_move_assignable_v<Type>)
            *dest = std::move(*source);
        else
            *dest = *source;
    }
};

#include "SPSCQueue.ipp"