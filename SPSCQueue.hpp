/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SPSC Queue
 */

#pragma once

#include <memory_resource>
#include <atomic>

namespace kF::Core
{
    template<typename Type, std::size_t Capacity>
    class SPSCQueue;
}

template<typename Type, std::size_t Capacity>
class kF::Core::SPSCQueue
{
public:
    SPSCQueue(void) noexcept : _buffer(reinterpret_cast<Type *>(std::malloc(sizeof(Type) * Capacity))) {}

    SPSCQueue(const SPSCQueue &other) = delete;
    SPSCQueue(SPSCQueue &&other) = delete;

    ~SPSCQueue(void) noexcept(std::is_nothrow_destructible_v<Type>) { clear(); std::free(_buffer); }

    template<typename ...Args>
    [[nodiscard]] bool push(Args &&...args) noexcept(std::is_nothrow_constructible_v<Type>)
    {
        const auto head = _head.load();
        const auto tail = _tail.load();
        const auto nextTail = Next(tail);

        if (nextTail == head)
            return false;
        new (_buffer + tail) Type(std::forward<Args>(args)...);
        _tail.store(nextTail);
        return true;
    }

    [[nodiscard]] bool pop(Type &value) noexcept(std::is_nothrow_move_assignable_v<Type>)
    {
        const auto head = _head.load();
        const auto tail = _tail.load();
        const auto nextHead = Next(head);

        if (head == tail)
            return false;
        value = std::move(_buffer[head]);
        _head.store(nextHead);
        return true;
    }

    void clear(void) noexcept(std::is_nothrow_destructible_v<Type>)
    {
        if constexpr (!std::is_trivially_destructible_v<Type>) {
            auto head = _head.load();
            auto tail = _tail.load();
            while (head != tail) {
                _buffer[head]->~Type();
                head = Next(head);
            }
            _head.store(0);
            _tail.store(0);
        }
    }

    // size()
    // push(vector)
    // pop(vector)

private:
    std::atomic<std::size_t> _head { 0 };
    std::atomic<std::size_t> _tail { 0 };
    Type *_buffer { nullptr };

    [[nodiscard]] static std::size_t Next(const std::size_t value) noexcept { return (value + 1) % Capacity; }
};