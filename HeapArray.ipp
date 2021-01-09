/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: HeapArray
 */

template<typename Type>
template<typename ...Args> requires std::constructible_from<Type, Args...>
void kF::Core::HeapArray<Type>::allocate(const std::size_t size, Args &&...args)
    noexcept(nothrow_ndebug && nothrow_constructible(Type, Args...) && nothrow_destructible(Type))
{
    if constexpr (!std::is_trivially_destructible_v<Type>) {
        for (auto &elem : *this)
            elem.~Type();
    }
    if (size && _size != size) [[likely]] {
        if (_size) [[unlikely]]
            Utils::AlignedFree(_data);
        _data = reinterpret_cast<Type *>(Utils::AlignedAlloc<alignof(Type)>(sizeof(Type) * size));
        kFAssert(_data,
            throw std::runtime_error("Core::HeapArray::allocate: Malloc failed"));
    }
    _size = size;
    for (auto i = 0ul; i < _size; ++i)
        new (&_data[i]) Type(args...);
}

template<typename Type>
inline void kF::Core::HeapArray<Type>::release(void) noexcept_destructible(Type)
{
    if (!_data) [[unlikely]]
        return;
    if constexpr (!std::is_trivially_destructible_v<Type>) {
        for (auto &elem : *this)
            elem.~Type();
    }
    Utils::AlignedFree(_data);
    _data = nullptr;
    _size = 0;
}
