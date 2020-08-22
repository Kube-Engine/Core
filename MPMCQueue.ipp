/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MPMC Queue
 */

#include "MPMCQueue.hpp"

template<typename Type>
kF::Core::MPMCQueue<Type>::MPMCQueue(const std::size_t capacity)
    : _buffer(Buffer { capacity - 1, nullptr })
{
    if (!((capacity >= 2) && ((capacity & (capacity - 1)) == 0)))
        throw std::invalid_argument("Core::MPMCQueue: Buffer capacity must be a power of 2");
    else if (_buffer.mask < 2)
        throw std::logic_error("Core::MPMCQueue: Capacity must be >= 2");
    else if (_buffer.data = reinterpret_cast<Cell *>(std::malloc(sizeof(Cell) * capacity)); !_buffer.data)
        throw std::runtime_error("Core::MPMCQueue: Malloc failed");
    for (auto i = 0ul; i < capacity; ++i)
        _buffer.data[i].sequence.store(i);
}

template<typename Type>
template<bool MoveOnSuccess, typename ...Args>
bool kF::Core::MPMCQueue<Type>::push(Args &&...args) noexcept(std::is_nothrow_constructible_v<Type, Args...>)
{
    auto pos = _tail.load(std::memory_order_relaxed);
    Cell *cell;

    while (true) {
        cell = &_buffer.data[pos & _buffer.mask];
        const auto sequence = cell->sequence.load(std::memory_order_acquire);
        if (sequence == pos) [[likely]] {
            if (_tail.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) [[likely]]
                break;
        } else if (sequence < pos) [[unlikely]]
            return false;
        else
            pos = _tail.load(std::memory_order_relaxed);
    }
    if constexpr (MoveOnSuccess)
        new (&cell->data) Type(std::move(args)...);
    else
        new (&cell->data) Type(std::forward<Args>(args)...);
    cell->sequence.store(pos + 1, std::memory_order_release);
    return true;
}

template<typename Type>
bool kF::Core::MPMCQueue<Type>::pop(Type &value) noexcept(kF::Core::Utils::NothrowCopyOrMoveAssign<Type>)
{
    auto pos = _head.load(std::memory_order_relaxed);
    Cell *cell;

    while (true) {
        cell = &_buffer.data[pos & _buffer.mask];
        const auto sequence = cell->sequence.load(std::memory_order_acquire);
        const auto next = pos + 1;
        if (sequence == next) [[likely]] {
            if (_head.compare_exchange_weak(pos, next, std::memory_order_relaxed)) [[likely]]
                break;
        } else if (sequence < next) [[unlikely]]
            return false;
        else
            pos = _head.load(std::memory_order_relaxed);
    }
    Utils::ForwardAssign(&value, &cell->data);
    cell->sequence.store(pos + _buffer.mask + 1, std::memory_order_release);
    return true;
}
