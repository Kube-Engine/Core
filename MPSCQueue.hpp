
/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MPSC Queue
 */

#pragma once

#include <memory_resource>
#include <atomic>

namespace kF::Core
{
    template<typename Type>
    class MPSCQueue;
}

template<typename Type>
class kF::Core::MPSCQueue
{
public:
    struct Node
    {
        std::atomic<Node> *next { nullptr };
        Type value;
    };

    MPSCQueue(void) noexcept
        : _pool()
    {
        auto *node = makeNode();
        _head = node;
        _tail = node;
    }

    MPSCQueue(const MPSCQueue &other) = delete;
    MPSCQueue(MPSCQueue &&other) = delete;

    ~MPSCQueue(void) noexcept(std::is_nothrow_destructible_v<Type>) { clear<true>(); }

    template<typename ...Args>
    void push(Args &&...args)
    {
        Node *toInsert = makeNode(std::forward<Args>(args)...);
        Node *expected = _tail.load();

        do {
            toInsert->next = expected;
        } while (!_tail.compare_exchange_weak(expected, toInsert));
    }

    [[nodiscard]] bool pop(Type &value)
    {
        auto head = _head.load(), tail = _tail.load();

        do {
            if (head == tail)
                return false;
        } while ()
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
    Node *_head;
    std::atomic<Node *> _tail;
    std::pmr::synchronized_pool_resource _pool;

    template<typename ...Args>
    [[nodiscard]] Node *makeNode(Args &&...args) noexcept {
        return new (_pool.allocate(sizeof(Node))) Node {
            next: nullptr,
            value: Type { std::forward<Args>(args)... }
        };
    }
};