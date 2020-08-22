/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SPSC Queue
 */

template<typename Type>
kF::Core::SPSCQueue<Type>::SPSCQueue(const std::size_t capacity)
{
    _buffer.capacity = capacity;
    if (_buffer.data = reinterpret_cast<Type *>(std::malloc(sizeof(Type) * capacity)); !_buffer.data)
        throw std::runtime_error("Core::SPSCQueue: Malloc failed");
}

template<typename Type>
template<typename ...Args>
inline bool kF::Core::SPSCQueue<Type>::push(Args &&...args) noexcept(std::is_nothrow_constructible_v<Type, Args...>)
{
    static_assert(std::is_constructible<Type, Args...>::value, "Type must be constructible from Args...");

    const auto tail = _tail.load(std::memory_order_relaxed);
    auto next = tail + 1;

    if (next == _buffer.capacity) [[unlikely]]
        next = 0;
    if (auto head = _headCache; next == head) [[unlikely]] {
        head = _headCache = _head.load(std::memory_order_acquire);
        if (next == head) [[unlikely]]
            return false;
    }
    new (_buffer.data + tail) Type{ std::forward<Args>(args)... };
    _tail.store(next, std::memory_order_release);
    return true;
}

template<typename Type>
template<bool ForceCopy>
std::size_t kF::Core::SPSCQueue<Type>::pushRange(Type *data, const std::size_t max) noexcept(kF::Core::Utils::NothrowCopyOrMoveConstruct<Type, ForceCopy>)
{
    return 0;
}

template<typename Type>
inline bool kF::Core::SPSCQueue<Type>::pop(Type &value) noexcept(kF::Core::Utils::NothrowCopyOrMoveAssign<Type>)
{
    const auto head = _head.load(std::memory_order_relaxed);

    if (auto tail = _tailCache; head == tail) [[unlikely]] {
        tail = _tailCache = _tail.load(std::memory_order_acquire);
        if (head == tail) [[unlikely]]
            return false;
    }
    auto *elem = reinterpret_cast<Type*>(_buffer.data + head);
    auto next = head + 1;
    if (next == _buffer.capacity) [[unlikely]]
        next = 0;
    value = std::move(*elem);
    elem->~Type();
    _head.store(next, std::memory_order_release);
    return true;
}

template<typename Type>
std::size_t kF::Core::SPSCQueue<Type>::popRange(Type *data, const std::size_t max) noexcept(kF::Core::Utils::NothrowCopyOrMoveAssign<Type>)
{
    return 0;
}

template<typename Type>
void kF::Core::SPSCQueue<Type>::clear(void) noexcept(std::is_nothrow_destructible_v<Type>)
{
    for (Type type; pop(type););
}
