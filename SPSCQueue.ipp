/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SPSC Queue
 */

template<typename Type>
kF::Core::SPSCQueue<Type>::SPSCQueue(const std::size_t capacity, const bool usedAsBuffer)
{
    _tailCache.buffer.capacity = capacity + usedAsBuffer;
    _tailCache.buffer.data = reinterpret_cast<Type *>(std::malloc(sizeof(Type) * _tailCache.buffer.capacity));
    if (!_tailCache.buffer.data)
        throw std::runtime_error("Core::SPSCQueue: Malloc failed");
    _headCache.buffer = _tailCache.buffer;
}

template<typename Type>
kF::Core::SPSCQueue<Type>::~SPSCQueue(void) noexcept_destructible(Type)
{
    clear();
    std::free(_tailCache.buffer.data);
}

template<typename Type>
template<typename ...Args>
inline bool kF::Core::SPSCQueue<Type>::push(Args &&...args) noexcept_constructible(Type, Args...)
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
inline bool kF::Core::SPSCQueue<Type>::pop(Type &value) noexcept(nothrow_destructible(Type) && (std::is_move_assignable_v<Type> ? nothrow_move_assignable(Type) : nothrow_move_constructible(Type)))
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
template<bool AllowLess, bool ForceCopy>
inline std::size_t kF::Core::SPSCQueue<Type>::pushRangeImpl(Type *data, const std::size_t count) noexcept(!ForceCopy && std::is_move_assignable_v<Type> ? nothrow_move_assignable(Type) : nothrow_move_constructible(Type))
{
    auto toPush = count;
    const auto tail = _tail.load(std::memory_order_relaxed);
    const auto capacity = _tailCache.buffer.capacity;
    auto head = _tailCache.value;
    auto available = capacity - (tail - head);

    if (available > capacity) [[unlikely]]
        available -= capacity;
    if (toPush >= available) [[unlikely]] {
        head = _tailCache.value = _head.load(std::memory_order_acquire);
        available = capacity - (tail - head);
        if (available > capacity) [[unlikely]]
            available -= capacity;
        if (toPush >= available) [[unlikely]] {
            if constexpr (AllowLess)
                toPush = available - 1;
            else
                return 0;
        }
    }
    auto next = tail + toPush;
    if (next >= capacity) [[unlikely]] {
        next -= capacity;
        const auto split = toPush - next;
        Utils::ForwardConstructRange<Type, ForceCopy, false>(_tailCache.buffer.data + tail, data, split);
        Utils::ForwardConstructRange<Type, ForceCopy, false>(_tailCache.buffer.data, data + split, next);
    } else {
        Utils::ForwardConstructRange<Type, ForceCopy, false>(_tailCache.buffer.data + tail, data, toPush);
    }
    _tail.store(next, std::memory_order_release);
    return toPush;
}

template<typename Type>
template<bool AllowLess>
inline std::size_t kF::Core::SPSCQueue<Type>::popRangeImpl(Type *data, const std::size_t count) noexcept(nothrow_destructible(Type) && (std::is_move_assignable_v<Type> ? nothrow_move_assignable(Type) : nothrow_move_constructible(Type)))
{
    auto toPop = count;
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
        if (toPop > available) [[unlikely]] {
            if constexpr (AllowLess)
                toPop = available;
            else
                return 0;
        }
    }
    auto next = head + toPop;
    if (next >= capacity) [[unlikely]] {
        next -= capacity;
        auto split = toPop - next;
        Utils::ForwardAssignRange<Type, false, true>(data, _headCache.buffer.data + head, split);
        Utils::ForwardAssignRange<Type, false, true>(data + split, _headCache.buffer.data, next);
    } else {
        Utils::ForwardAssignRange<Type, false, true>(data, _headCache.buffer.data + head, toPop);
    }
    _head.store(next, std::memory_order_release);
    return toPop;
}

template<typename Type>
void kF::Core::SPSCQueue<Type>::clear(void) noexcept_destructible(Type)
{
    for (Type type; pop(type););
}
