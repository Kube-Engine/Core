/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: MPMC Queue
 */

#include <stdexcept>

template<typename Type>
kF::Core::MPMCQueue<Type>::MPMCQueue(const std::size_t capacity)
    : _tailCache(Cache { Buffer { capacity - 1, nullptr } })
{
    if (!((capacity >= 2) && ((capacity & (capacity - 1)) == 0)))
        throw std::invalid_argument("Core::MPMCQueue: Buffer capacity must be a power of 2");
    else if (_tailCache.buffer.mask < 2)
        throw std::logic_error("Core::MPMCQueue: Capacity must be >= 2");
    else if (_tailCache.buffer.data = reinterpret_cast<Cell *>(std::malloc(sizeof(Cell) * capacity)); !_tailCache.buffer.data)
        throw std::runtime_error("Core::MPMCQueue: Malloc failed");
    for (auto i = 0ul; i < capacity; ++i)
        _tailCache.buffer.data[i].sequence.store(i);
    _headCache = _tailCache;
}

template<typename Type>
kF::Core::MPMCQueue<Type>::~MPMCQueue(void) noexcept_destructible(Type)
{
    clear();
    std::free(_tailCache.buffer.data);
}

template<typename Type>
template<bool MoveOnSuccess, typename ...Args>
inline bool kF::Core::MPMCQueue<Type>::push(Args &&...args)
    noexcept_constructible(Type, Args...)
    requires std::constructible_from<Type, Args...>
{
    auto pos = _tail.load(std::memory_order_relaxed);
    auto * const data = _tailCache.buffer.data;
    const auto mask = _tailCache.buffer.mask;
    Cell *cell;

    while (true) {
        cell = &data[pos & mask];
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
inline bool kF::Core::MPMCQueue<Type>::pop(Type &value)
    noexcept(nothrow_destructible(Type) && nothrow_forward_constructible(Type))
{
    auto pos = _head.load(std::memory_order_relaxed);
    const auto mask = _headCache.buffer.mask;
    auto * const data = _headCache.buffer.data;
    Cell *cell;

    while (true) {
        cell = &data[pos & mask];
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
    if constexpr (std::is_move_assignable_v<Type>)
        value = std::move(cell->data);
    else
        value = cell->data;
    cell->data.~Type();
    cell->sequence.store(pos + mask + 1, std::memory_order_release);
    return true;
}
