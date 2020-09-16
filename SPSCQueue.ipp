/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SPSC Queue
 */

template<typename Type>
kF::Core::SPSCQueue<Type>::SPSCQueue(const std::size_t capacity, const bool usedAsBuffer)
{
    _tailCache.buffer.capacity = capacity + usedAsBuffer;
    if (_tailCache.buffer.data = reinterpret_cast<Type *>(std::malloc(sizeof(Type) * _tailCache.buffer.capacity)); !_tailCache.buffer.data)
        throw std::runtime_error("Core::SPSCQueue: Malloc failed");
    _headCache.buffer = _tailCache.buffer;
}

template<typename Type>
kF::Core::SPSCQueue<Type>::~SPSCQueue(void) noexcept(std::is_nothrow_destructible_v<Type>)
{
    clear();
    std::free(_tailCache.buffer.data);
}

template<typename Type>
template<typename ...Args>
bool kF::Core::SPSCQueue<Type>::push(Args &&...args) noexcept(std::is_nothrow_constructible_v<Type, Args...>)
{
    static_assert(std::is_constructible<Type, Args...>::value, "Type must be constructible from Args...");

    const auto tail = _tail.load(std::memory_order_relaxed);
    auto next = tail + 1;

    if (next == _tailCache.buffer.capacity) [[unlikely]]
        next = 0;
    if (auto head = _tailCache.value; next == head) [[unlikely]] {
        head = _tailCache.value = _head.load(std::memory_order_acquire);
        if (next == head) [[unlikely]]
            return false;
    }
    new (_tailCache.buffer.data + tail) Type { std::forward<Args>(args)... };
    _tail.store(next, std::memory_order_release);
    return true;
}

template<typename Type>
bool kF::Core::SPSCQueue<Type>::pop(Type &value) noexcept(kF::Core::Utils::NothrowCopyOrMoveAssign<Type>)
{
    const auto head = _head.load(std::memory_order_relaxed);

    if (auto tail = _headCache.value; head == tail) [[unlikely]] {
        tail = _headCache.value = _tail.load(std::memory_order_acquire);
        if (head == tail) [[unlikely]]
            return false;
    }
    auto *elem = reinterpret_cast<Type *>(_headCache.buffer.data + head);
    auto next = head + 1;
    if (next == _headCache.buffer.capacity) [[unlikely]]
        next = 0;
    Utils::ForwardAssign<Type, false, true>(&value, elem);
    _head.store(next, std::memory_order_release);
    return true;
}

template<typename Type>
template<bool ForceCopy>
bool kF::Core::SPSCQueue<Type>::pushRange(Type *data, const std::size_t count) noexcept(kF::Core::Utils::NothrowCopyOrMoveConstruct<Type, ForceCopy>)
{
    const auto tail = _tail.load(std::memory_order_relaxed);
    const auto capacity = _tailCache.buffer.capacity;
    auto head = _tailCache.value;
    auto available = capacity - (tail - head);

    if (available > capacity) [[unlikely]]
        available -= capacity;
    if (count >= available) [[unlikely]] {
        head = _tailCache.value = _head.load(std::memory_order_acquire);
        available = capacity - (tail - head);
        if (available > capacity) [[unlikely]]
            available -= capacity;
        if (count >= available) [[unlikely]]
            return false;
    }
    auto next = tail + count;
    if (next >= capacity) [[unlikely]] {
        next -= capacity;
        const auto split = count - next;
        Utils::ForwardConstructRange<Type, ForceCopy, false>(_tailCache.buffer.data + tail, data, split);
        Utils::ForwardConstructRange<Type, ForceCopy, false>(_tailCache.buffer.data, data + split, next);
    } else {
        Utils::ForwardConstructRange<Type, ForceCopy, false>(_tailCache.buffer.data + tail, data, count);
    }
    _tail.store(next, std::memory_order_release);
    return true;
//     const auto tail = _tail.load(std::memory_order_relaxed);
//     const auto next = (tail + count) % _tailCache.buffer.capacity;
//     auto head = _tailCache.value;
//     auto available = (_tailCache.buffer.capacity - 1) - (head < tail ? tail - head : head - tail);

//     if (available < count) [[unlikely]] {
//         head = _tailCache.value = _head.load(std::memory_order_acquire);
//         available = (_tailCache.buffer.capacity - 1) - (head < tail ? tail - head : head - tail);
//         if (available < count) [[unlikely]]
//             return false;
//     }
//     if (next > tail) {
//         for (auto x = 0ul; x < count; ++x)
//             Utils::ForwardConstruct<Type, ForceCopy>(_tailCache.buffer.data + x, data + x);
//     } else {
//         auto x = tail, y = 0ul;
//         for (; x != _tailCache.buffer.capacity; ++x, ++y)
//             Utils::ForwardConstruct<Type, ForceCopy>(_tailCache.buffer.data + x, data + y);
//         for (x = 0ul; x != next; ++x, ++y)
//             Utils::ForwardConstruct<Type, ForceCopy>(_tailCache.buffer.data + x, data + y);
//     }
//     _tail.store(next, std::memory_order_release);
//     return true;
}

template<typename Type>
bool kF::Core::SPSCQueue<Type>::popRange(Type *data, const std::size_t count) noexcept(kF::Core::Utils::NothrowCopyOrMoveAssign<Type>)
{
    const auto head = _head.load(std::memory_order_relaxed);
    const auto capacity = _headCache.buffer.capacity;
    auto tail = _headCache.value;
    auto available = tail - head;

    if (available > capacity) [[unlikely]]
        available += capacity;
    if (count >= available) [[unlikely]] {
        tail = _headCache.value = _tail.load(std::memory_order_acquire);
        available = tail - head;
        if (available > capacity) [[unlikely]]
            available += capacity;
        if (count > available) [[unlikely]]
            return false;
    }
    auto next = head + count;
    if (next >= capacity) [[unlikely]] {
        next -= capacity;
        auto split = count - next;
        Utils::ForwardAssignRange<Type, false, true>(data, _headCache.buffer.data + head, split);
        Utils::ForwardAssignRange<Type, false, true>(data + split, _headCache.buffer.data, next);
    } else {
        Utils::ForwardAssignRange<Type, false, true>(data, _headCache.buffer.data + head, count);
    }
    _head.store(next, std::memory_order_release);
    return true;
    // const auto head = _head.load(std::memory_order_relaxed);
    // const auto next = (head + count) % _headCache.buffer.capacity;
    // auto tail = _headCache.value;
    // auto available = (tail < head ? head - tail : tail - head);

    // if (available < count) [[unlikely]] {
    //     tail = _headCache.value = _tail.load(std::memory_order_acquire);
    //     available = (tail < head ? head - tail : tail - head);
    //     if (available < count) [[unlikely]]
    //         return false;
    // }
    // if (next > head) {
    //     for (auto x = 0ul; x < count; ++x)
    //         Utils::ForwardAssign(data + x, _headCache.buffer.data + x);
    // } else {
    //     auto x = head, y = 0ul;
    //     for (; x != _headCache.buffer.capacity; ++x, ++y)
    //         Utils::ForwardAssign(data + y, _headCache.buffer.data + x);
    //     for (x = 0ul; x != next; ++x, ++y)
    //         Utils::ForwardAssign(data + y, _headCache.buffer.data + x);
    // }
    // _head.store(next, std::memory_order_release);
    // return true;
}

template<typename Type>
void kF::Core::SPSCQueue<Type>::clear(void) noexcept(std::is_nothrow_destructible_v<Type>)
{
    for (Type type; pop(type););
}
