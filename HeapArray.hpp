/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A dead simple runtime array
 */

#pragma once

#include <memory>

#include "Assert.hpp"

namespace kF::Core
{
    template<typename Type>
    class HeapArray;
}

template<typename Type>
class kF::Core::HeapArray
{
public:
    using Iterator = Type *;
    using ConstIterator = const Type *;

    /** @brief Default construct an empty array */
    HeapArray(void) = default;

    /** @brief Construct an array of a given size with given arguments */
    template<typename ...Args>
    HeapArray(const std::size_t size, Args &&...args) noexcept(!KUBE_DEBUG_BUILD && std::is_nothrow_constructible_v<Type, Args...>) { allocate(size, args...); }

    /** @brief Move constructor */
    HeapArray(HeapArray &&other) noexcept { swap(other); }

    /** @brief Destruct the array and all elements */
    ~HeapArray(void) { if (!empty()) clear(); }

    /** @brief Fast check if array contains data */
    operator bool(void) const noexcept { return empty(); }
    [[nodiscard]] bool empty(void) const noexcept { return _size; }

    /** @brief Allocate a new array */
    template<bool Initialize = true, typename ...Args>
    void allocate(const std::size_t size, Args &&...args) noexcept(!KUBE_DEBUG_BUILD && (!Initialize || std::is_nothrow_default_constructible_v<Type>) && std::is_nothrow_destructible_v<Type>)
    {
        if constexpr (!std::is_trivially_destructible_v<Type>) {
            for (auto i = 0ul; i < _size; ++i)
                destroy(i);
        }
        if (size && _size != size) [[likely]] {
            if (_size) [[unlikely]]
                std::free(_data);
            _data = reinterpret_cast<Type *>(std::malloc(sizeof(Type) * size));
            kFAssert(_data,
                throw std::runtime_error("Core::HeapArray: Invalid malloc"));
        }
        _size = size;
        if constexpr (Initialize) {
            for (auto i = 0ul; i < _size; ++i)
                init(i, std::forward<Args>(args)...);
        }
    }

    /** @brief Clear the array */
    void clear(void)
    {
        if constexpr (!std::is_trivially_destructible_v<Type>) {
            for (auto i = 0ul; i < _size; ++i)
                destroy(i);
        }
        std::free(_data);
    }

    /** @brief Initialize an element of the array */
    template<typename ...Args>
    void init(const std::size_t index, Args &&...args) noexcept(std::is_nothrow_constructible_v<Type, Args...>) { new (&at(index)) Type(std::forward<Args>(args)...); }

    /** @brief Destroy an element of the array */
    void destroy(const std::size_t index) noexcept(std::is_nothrow_destructible_v<Type>) { at(index).~Type(); }

    /** @brief Get internal data */
    [[nodiscard]] Type *data(void) noexcept { return _data.get(); }
    [[nodiscard]] const Type *data(void) const noexcept { return _data.get(); }

    /** @brief Get array length */
    [[nodiscard]] std::size_t size(void) const noexcept { return _size; }

    /** @brief Helpers to access data */
    [[nodiscard]] Type &at(const std::size_t index) noexcept { return _data[index]; }
    [[nodiscard]] const Type &at(const std::size_t index) const noexcept { return _data[index]; }
    [[nodiscard]] Type &operator[](const std::size_t index) noexcept { return _data[index]; }
    [[nodiscard]] const Type &operator[](const std::size_t index) const noexcept { return _data[index]; }

    /** Iterators */
    [[nodiscard]] Iterator begin(void) noexcept { return data(); }
    [[nodiscard]] Iterator end(void) noexcept { return data() + size(); }
    [[nodiscard]] ConstIterator begin(void) const noexcept { return data(); }
    [[nodiscard]] ConstIterator end(void) const noexcept { return data() + size(); }

    /** @brief Swap two instances */
    void swap(HeapArray &other) noexcept { std::swap(_data, other._data); std::swap(_size, other._size); }

private:
    Type *_data {};
    std::size_t _size { 0 };
};