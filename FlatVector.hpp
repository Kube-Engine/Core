/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

#pragma once

#include <memory>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Type>
    class FlatVector;
}

/** @brief Flat vector is pointer-sized vector alternative
 * The implementation has same performances as std::vector but comes with a single weakness :
 * Because the size and capacity are stored on the heap if you wish to get the vector size and not lookup after that
 * it is slower due to memory indirection
*/
template<typename Type>
class kF::Core::FlatVector
{
public:
    struct alignas(sizeof(Type) <= 16 ? 16 : 64) Header
    {
        std::size_t size { 0 };
        std::size_t capacity { 0 };
    };

    /** @brief Iterators */
    using Iterator = Type *;
    using ConstIterator = const Type *;

    /** @brief Default constructor */
    FlatVector(void) noexcept = default;

    /** @brief Copy constructor */
    FlatVector(const FlatVector &other) noexcept_copy_constructible(Type) { resize(other.begin(), other.end()); }

    /** @brief Move constructor */
    FlatVector(FlatVector &&other) noexcept { swap(other); }

    /** @brief Insert constructor */
    template<std::input_iterator InputIterator>
    FlatVector(const InputIterator from, const InputIterator to)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { resize(from, to); }

    /** @brief Resize with default constructor */
    FlatVector(const std::size_t count)
        noexcept(nothrow_constructible(Type) && nothrow_destructible(Type))
        requires std::copy_constructible<Type>
        { resize(count); }

    /** @brief Resize with copy constructor */
    FlatVector(const std::size_t count, const Type &value)
        noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type))
        requires std::copy_constructible<Type>
        { resize(count, value); }

    /** @brief Initializer list constructor */
    FlatVector(std::initializer_list<Type> init) noexcept_forward_constructible(Type)
        : FlatVector(init.begin(), init.end()) {}

    /** @brief Destroy constructor */
    ~FlatVector(void) noexcept_destructible(Type) { release(); }

    /** @brief Copy assignment */
    FlatVector &operator=(const FlatVector &other) noexcept_copy_constructible(Type) { resize(other.begin(), other.end()); return *this; }

    /** @brief Move assignment */
    FlatVector &operator=(FlatVector &&other) noexcept { swap(other); return *this; }

    /** @brief Swap two instances */
    void swap(FlatVector &other) noexcept { std::swap(_ptr, other._ptr); }

    /** @brief Get raw data pointer */
    [[nodiscard]] Type *data(void) noexcept { return reinterpret_cast<Type *>(_ptr + 1); }
    [[nodiscard]] const Type *data(void) const noexcept { return reinterpret_cast<const Type *>(_ptr + 1); }

    /** @brief Access an element of the vector */
    [[nodiscard]] Type &at(const std::size_t index) noexcept { return data()[index]; }
    [[nodiscard]] const Type &at(const std::size_t index) const noexcept { return data()[index]; }

    /** @brief Access operator */
    [[nodiscard]] Type &operator[](const std::size_t index) noexcept { return at(index); }
    [[nodiscard]] const Type &operator[](const std::size_t index) const noexcept { return at(index); }

    /** @brief Fast check if vector contains data */
    [[nodiscard]] bool empty(void) const noexcept { return !_ptr || !_ptr->size; }
    operator bool(void) const noexcept { return !empty(); }

    /** @brief Get size of vector */
    [[nodiscard]] std::size_t size(void) const noexcept;
    [[nodiscard]] std::size_t sizeUnsafe(void) const noexcept { return _ptr->size; }

    /** @brief Get capacity of vector */
    [[nodiscard]] std::size_t capacity(void) const noexcept;
    [[nodiscard]] std::size_t capacityUnsafe(void) const noexcept { return _ptr->capacity; }

    /** @brief Begin / end iterators */
    [[nodiscard]] Iterator begin(void) noexcept { return const_cast<Iterator>(const_cast<const FlatVector<Type> &>(*this).begin()); }
    [[nodiscard]] ConstIterator begin(void) const noexcept;
    [[nodiscard]] ConstIterator cbegin(void) const noexcept { return begin(); }
    [[nodiscard]] Iterator end(void) noexcept { return const_cast<Iterator>(const_cast<const FlatVector<Type> &>(*this).end()); }
    [[nodiscard]] ConstIterator end(void) const noexcept;
    [[nodiscard]] ConstIterator cend(void) const noexcept { return end(); }
    [[nodiscard]] Iterator beginUnsafe(void) noexcept { return data(); }
    [[nodiscard]] ConstIterator beginUnsafe(void) const noexcept { return data(); }
    [[nodiscard]] Iterator endUnsafe(void) noexcept { return data() + sizeUnsafe(); }
    [[nodiscard]] ConstIterator endUnsafe(void) const noexcept { return data() + sizeUnsafe(); }

    /** @brief Front / Back getters */
    [[nodiscard]] Type &front(void) noexcept { return at(0); }
    [[nodiscard]] const Type &front(void) const noexcept { return at(0); }
    [[nodiscard]] Type &back(void) noexcept { return at(sizeUnsafe() - 1); }
    [[nodiscard]] const Type &back(void) const noexcept { return at(sizeUnsafe() - 1); }

    /** @brief Push an element into the vector */
    template<typename ...Args>
    Type &push(Args &&...args)
        noexcept(std::is_nothrow_constructible_v<Type, Args...> && nothrow_destructible(Type))
        requires std::constructible_from<Type, Args...>;

    /** @brief Push the last element of the vector */
    void pop(void) noexcept_destructible(Type);

    /** @brief Insert an initializer list */
    Iterator insert(const Iterator pos, std::initializer_list<Type> &&init)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { return insert(pos, init.begin(), init.end()); }

    /** @brief Insert a range of element by iterating over iterators */
    template<std::input_iterator InputIterator>
    Iterator insert(const Iterator pos, const InputIterator from, const InputIterator to)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        requires std::constructible_from<Type, decltype(*std::declval<InputIterator>())>;

    /** @brief Insert a range of copies */
    Iterator insert(const Iterator pos, const std::size_t count, const Type &value)
        noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type))
        requires std::copy_constructible<Type>;

    /** @brief Remove a range of elements */
    void erase(const Iterator from, const Iterator to)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type));

    /** @brief Remove a range of elements */
    void erase(const Iterator from, const std::size_t count)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { erase(from, from + count); }

    /** @brief Remove a specific element */
    void erase(const Iterator pos)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { erase(pos, pos + 1); }

    /** @brief Resize the vector using default constructor to initialize each element */
    void resize(const std::size_t count)
        noexcept(std::is_nothrow_constructible_v<Type> && nothrow_destructible(Type))
        requires std::constructible_from<Type>;

    /** @brief Resize the vector by copying given element */
    void resize(const std::size_t count, const Type &type)
        noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type))
        requires std::copy_constructible<Type>;

    /** @brief Resize the vector with input iterators */
    template<std::input_iterator InputIterator>
    void resize(const InputIterator from, const InputIterator to)
        noexcept(nothrow_destructible(Type) && nothrow_forward_iterator_constructible(InputIterator));

    /** @brief Reserve memory for fast emplace */
    void reserve(const std::size_t count) noexcept_destructible(Type);

    /** @brief Clear the vector */
    template<bool SafeCheck = true>
    void clear(void) noexcept_destructible(Type);

    /** @brief Clear vector and release memory */
    template<bool SafeCheck = true>
    void release(void) noexcept_destructible(Type);

private:
    Header *_ptr { nullptr };

    /** @brief Grow the vector */
    void grow(const std::size_t minimum = 0) noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type));
};

#include "FlatVector.ipp"

static_assert(sizeof(kF::Core::FlatVector<int>::Header) == 16);
static_assert(sizeof(kF::Core::FlatVector<char[16]>::Header) == 16);
static_assert(sizeof(kF::Core::FlatVector<char[17]>::Header) == 64);