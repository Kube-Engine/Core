/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SPMC Queue
 */

#pragma once

#include <memory_resource>
#include <atomic>

namespace kF::Core
{
    template<typename Type>
    class SPMCQueue;
}

template<typename Type, std::size_t Capacity>
class kF::Core::SPMCQueue
{
public:
    struct Node
    {
        std::atomic<Node> *next { nullptr };
        Type value;
    };

    SPMCQueue(void) noexcept
        : _pool()
    {
        auto *node = makeNode();
        _head = node;
        _tail = node;
    }

    SPMCQueue(const SPMCQueue &other) = delete;
    SPMCQueue(SPMCQueue &&other) = delete;

    ~SPMCQueue(void) noexcept(std::is_nothrow_destructible_v<Type>) { clear<true>(); }

    template<typename ...Args>
    [[nodiscard]] bool push(Args &&...args)
    {
        std::size_t head, newTail, tail = _futureTail.load();

        do {
            newTail = (tail + 1 % Capacity);
            if (newTail == _head.load())
                return false;
        } while (!_futureTail.compare_exchange_strong(tail, newTail));
        ++_tail;
        return true;

        if (std::abs(tail - _head.load()))

        head = _head.load(), tail = _futureTail.load();
        do {
            size = std::abs(tail - head)

        if (!size)
            return false;
        } while (_futureTail.compare_exchange_strong(tail, ))
    }

    [[nodiscard]] bool pop(Type &value)
    {
    }

    template<bool Destruct = false>
    void clear(void) noexcept(std::is_nothrow_destructible_v<Type>)
    {
        auto it = _head.load();
        decltype(it) next;

        if constexpr (Destruct)
            next = it->next;
        else {
            it = it->next;
            next = it;
        }
        while (it) {
            next = it->next;
            it->~Node();
            _pool.deallocate(it, sizeof(Node));
            it = next;
        }
    }

private:
    std::atomic<std::size_t> _head { 0 };
    std::atomic<std::size_t> _tail { 0 };
    std::unique_ptr<std::atomic<Type*>[Capacity]> _buffer {};
};