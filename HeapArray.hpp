/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A dead simple runtime array
 */

#pragma once

#include <memory>

#include "Assert.hpp"
#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type>
    class HeapArray;
}

template<typename Type>
class alignas_quarter_cacheline kF::Core::HeapArray
{
public:
    using Iterator = Type *;
    using ConstIterator = const Type *;

    /** @brief Default construct an empty array */
    HeapArray(void) noexcept = default;

    /** @brief Construct an array of a given size with given arguments */
    template<typename ...Args>
    HeapArray(const std::size_t size, Args &&...args) noexcept(nothrow_ndebug && nothrow_constructible(Type, Args...)) { allocate(size, args...); }

    /** @brief Move constructor */
    HeapArray(HeapArray &&other) noexcept { swap(other); }

    /** @brief Destruct the array and all elements */
    ~HeapArray(void) noexcept { release(); }

    /** @brief Move assignment */
    HeapArray &operator=(HeapArray &&other) noexcept { swap(other); return *this; }

    /** @brief Swap two instances */
    void swap(HeapArray &other) noexcept { std::swap(_data, other._data); std::swap(_size, other._size); }


    /** @brief Fast check if array contains data */
    operator bool(void) const noexcept { return !empty(); }
    [[nodiscard]] bool empty(void) const noexcept { return !_size; }


    /** @brief Allocate a new array */
    template<typename ...Args> requires std::constructible_from<Type, Args...>
    void allocate(const std::size_t size, Args &&...args)
        noexcept(nothrow_ndebug && nothrow_constructible(Type, Args...) && nothrow_destructible(Type));

    /** @brief Clear the array and release memory */
    void release(void) noexcept_destructible(Type);


    /** @brief Get internal data */
    [[nodiscard]] Type *data(void) noexcept { return _data; }
    [[nodiscard]] const Type *data(void) const noexcept { return _data; }


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

private:
    Type *_data { nullptr };
    std::size_t _size { 0 };
};

#include "HeapArray.ipp"