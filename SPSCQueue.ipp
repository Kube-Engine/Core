/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SPSC Queue
 */

template<typename Type, std::size_t Capacity>
template<typename ...Args>
bool kF::Core::SPSCQueue<Type, Capacity>::push(Args &&...args) noexcept(std::is_nothrow_constructible_v<Type, Args...>)
{
    const auto size = _size.load();
    const auto tail = _tail;
    const auto nextTail = Next(tail);

    if (size == Capacity)
        return false;
    new (_buffer + tail) Type(std::forward<Args>(args)...);
    _tail = nextTail;
    ++_size;
    return true;
}

template<typename Type, std::size_t Capacity>
template<bool ForceCopy>
std::size_t kF::Core::SPSCQueue<Type, Capacity>::pushRange(Type *data, const std::size_t max) noexcept(kF::Core::SPSCQueueUtils::NothrowCopyOrMoveConstruct<Type, ForceCopy>)
{
    const auto tail = _tail;
    const auto available = Capacity - _size.load();
    const auto toInsert = max > available ? available : max;
    const auto nextTail = Increment(tail, toInsert);
    const auto range1 = tail + toInsert > Capacity ? Capacity - tail : toInsert;
    const auto range2 = toInsert - range1;

    if (!toInsert)
        return 0;
    for (auto i = 0ul; i < range1; ++i)
        ForwardConstruct<ForceCopy>(_buffer + (tail + i), data + i);
    for (auto i = 0ul; i < range2; ++i)
        ForwardConstruct<ForceCopy>(_buffer + i, data + i);
    _tail = nextTail;
    _size += toInsert;
    return toInsert;
}

template<typename Type, std::size_t Capacity>
bool kF::Core::SPSCQueue<Type, Capacity>::pop(Type &value) noexcept(kF::Core::SPSCQueueUtils::NothrowCopyOrMoveAssign<Type>)
{
    const auto head = _head;
    const auto size = _size.load();
    const auto nextHead = Next(head);

    if (!size)
        return false;
    ForwardAssign(&value, _buffer + head);
    _head = nextHead;
    --_size;
    return true;
}

template<typename Type, std::size_t Capacity>
std::size_t kF::Core::SPSCQueue<Type, Capacity>::popRange(Type *data, const std::size_t max) noexcept(kF::Core::SPSCQueueUtils::NothrowCopyOrMoveAssign<Type>)
{
    const auto head = _head;
    const auto size = _size.load();
    const auto toExtract = max > size ? size : max;
    const auto nextHead = Increment(head, toExtract);
    const auto range1 = head + toExtract > Capacity ? Capacity - head : toExtract;
    const auto range2 = toExtract - range1;

    if (!toExtract)
        return 0;
    for (auto i = 0ul; i < range1; ++i)
        ForwardAssign(data + i, _buffer + (head + i));
    for (auto i = 0ul; i < range2; ++i)
        ForwardAssign(data + i, _buffer + i);
    _head = nextHead;
    _size -= toExtract;
    return toExtract;
}

template<typename Type, std::size_t Capacity>
void kF::Core::SPSCQueue<Type, Capacity>::clear(void) noexcept(std::is_nothrow_destructible_v<Type>)
{
    if constexpr (!std::is_trivially_destructible_v<Type>) {
        const auto head = _head;
        const auto toExtract = _size.load();
        const auto range1 = head + toExtract >= Capacity ? Capacity - head : toExtract;
        const auto range2 = toExtract - range1;

        for (auto i = 0; i < range1; ++i)
            _buffer[head + i].~Type();
        for (auto i = 0; i < range2; ++i)
            _buffer[i].~Type();
    }
    _head = 0;
    _tail = 0;
    _size.store(0);
}
