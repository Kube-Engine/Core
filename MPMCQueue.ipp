/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MPMC Queue
 */

#include "MPMCQueue.hpp"


template<typename Type, std::size_t Capacity, bool PackMembers>
kF::Core::MPMCQueue<Type, Capacity, PackMembers>::MPMCQueue(void) noexcept
    :   _buffer(reinterpret_cast<Type *>(std::malloc(sizeof(Type) * Capacity + sizeof(State) * Capacity))),
        _states(reinterpret_cast<State *>(std::malloc(sizeof(State) * Capacity)))
{
    for (auto i = 0ul; i < Capacity; ++i)
        _states[i].sequence.store(i);
}

template<typename Type, std::size_t Capacity, bool PackMembers>
template<typename ...Args>
bool kF::Core::MPMCQueue<Type, Capacity, PackMembers>::push(Args &&...args) noexcept(std::is_nothrow_constructible_v<Type, Args...>)
{
    std::size_t tail, realTail;

    while (true) {
        tail = _tail.load();
        realTail = tail % Capacity;
        auto sequence = _states[realTail].sequence.load();
        if (sequence == tail && _tail.compare_exchange_weak(tail, tail + 1))
            break;
        else if (sequence < tail)
            return false;
    }
    new (_buffer + realTail) Type(std::forward<Args>(args)...);
    ++_states[realTail].sequence;
    return true;
}

template<typename Type, std::size_t Capacity, bool PackMembers>
bool kF::Core::MPMCQueue<Type, Capacity, PackMembers>::pop(Type &value) noexcept(kF::Core::Utils::NothrowCopyOrMoveAssign<Type>)
{
    std::size_t head, realHead;

    while (true) {
        head = _head.load();
        realHead = head % Capacity;
        const auto nextHead = head + 1;
        auto sequence = _states[realHead].sequence.load();
        if (sequence == nextHead && _head.compare_exchange_weak(head, nextHead))
            break;
        else if (sequence < nextHead)
            return false;
    }
    Utils::ForwardAssign(&value, _buffer + realHead);
    _states[realHead].sequence.store(head + Capacity);
    return true;
}
