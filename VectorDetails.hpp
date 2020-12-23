/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: VectorDetails
 */

#pragma once

#include <initializer_list>
#include <memory>

#include "Assert.hpp"
#include "Utils.hpp"

namespace kF::Core::Internal
{
    template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized = false>
    class VectorDetails;
}

template<typename Base, typename Type, std::integral Range, bool IsSmallOptimized>
class kF::Core::Internal::VectorDetails : public Base
{
public:
    /** @brief Iterator detectors */
    using Iterator = decltype(std::declval<Base &>().begin());
    using ConstIterator = decltype(std::declval<const Base &>().begin());

    /** @brief All required base functions */
    using Base::data;
    using Base::dataUnsafe;
    using Base::setData;
    using Base::size;
    using Base::sizeUnsafe;
    using Base::setSize;
    using Base::capacity;
    using Base::capacityUnsafe;
    using Base::setCapacity;
    using Base::begin;
    using Base::beginUnsafe;
    using Base::end;
    using Base::endUnsafe;
    using Base::allocate;
    using Base::deallocate;
    using Base::empty;
    using Base::swap;

    /** @brief Default constructor */
    VectorDetails(void) noexcept = default;

    /** @brief Copy constructor */
    VectorDetails(const VectorDetails &other) noexcept_copy_constructible(Type)
        { resize(other.begin(), other.end()); }

    /** @brief Move constructor */
    VectorDetails(VectorDetails &&other) noexcept { swap(other); }

    /** @brief Resize with default constructor */
    VectorDetails(const Range count)
        noexcept(nothrow_constructible(Type) && nothrow_destructible(Type))
        requires std::default_initializable<Type>
        { resize(count); }

    /** @brief Resize with copy constructor */
    VectorDetails(const Range count, const Type &value)
        noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type))
        requires std::copy_constructible<Type>
        { resize(count, value); }

    /** @brief Insert constructor */
    template<std::input_iterator InputIterator>
    VectorDetails(const InputIterator from, const InputIterator to)
        noexcept(nothrow_forward_iterator_constructible(InputIterator) && nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { resize(from, to); }

    /** @brief Initializer list constructor */
    VectorDetails(std::initializer_list<Type> &&init) noexcept_forward_constructible(Type)
        : VectorDetails(init.begin(), init.end()) {}

    /** @brief Release the vector */
    ~VectorDetails(void) noexcept_destructible(Type) { release(); }

    /** @brief Copy assignment */
    VectorDetails &operator=(const VectorDetails &other) noexcept_copy_constructible(Type)
        { resize(other.begin(), other.end()); return *this; }

    /** @brief Move assignment */
    VectorDetails &operator=(VectorDetails &&other) noexcept { swap(other); return *this; }

    /** @brief Fast non-empty check */
    [[nodiscard]] operator bool(void) const noexcept { return !empty(); }


    /** @brief Begin / End helpers */
    [[nodiscard]] ConstIterator cbegin(void) const noexcept { return begin(); }
    [[nodiscard]] ConstIterator cend(void) const noexcept { return end(); }
    [[nodiscard]] Iterator rbegin(void) noexcept { return std::make_reverse_iterator(begin()); }
    [[nodiscard]] Iterator rend(void) noexcept { return std::make_reverse_iterator(end()); }
    [[nodiscard]] ConstIterator rbegin(void) const noexcept { return std::make_reverse_iterator(begin()); }
    [[nodiscard]] ConstIterator rend(void) const noexcept { return std::make_reverse_iterator(end()); }
    [[nodiscard]] ConstIterator crbegin(void) const noexcept { return rbegin(); }
    [[nodiscard]] ConstIterator crend(void) const noexcept { return rend(); }


    /** @brief Access element at positon */
    [[nodiscard]] Type &at(const Range pos ) noexcept { return data()[pos]; }
    [[nodiscard]] const Type &at(const Range pos) const noexcept { return data()[pos]; }

    /** @brief Access element at positon */
    [[nodiscard]] Type &operator[](const Range pos ) noexcept { return data()[pos]; }
    [[nodiscard]] const Type &operator[](const Range pos) const noexcept { return data()[pos]; }

    /** @brief Get first element */
    [[nodiscard]] Type &front(void) noexcept { return at(0); }
    [[nodiscard]] const Type &front(void) const noexcept { return at(0); }

    /** @brief Get last element */
    [[nodiscard]] Type &back(void) noexcept { return at(sizeUnsafe() - 1); }
    [[nodiscard]] const Type &back(void) const noexcept { return at(sizeUnsafe() - 1); }


    /** @brief Push an element into the vector */
    template<typename ...Args>
    Type &push(Args &&...args)
        noexcept(std::is_nothrow_constructible_v<Type, Args...> && nothrow_destructible(Type))
        requires std::constructible_from<Type, Args...>;

    /** @brief Pop the last element of the vector */
    void pop(void) noexcept_destructible(Type);


    /** @brief Insert an initializer list */
    Iterator insert(const Iterator pos, std::initializer_list<Type> &&init)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { return insert(pos, init.begin(), init.end()); }

    /** @brief Insert a range of copies */
    Iterator insert(const Iterator pos, const Range count)
        noexcept(nothrow_constructible(Type) && nothrow_destructible(Type));

    /** @brief Insert a range of copies */
    Iterator insert(const Iterator pos, const Range count, const Type &value)
        noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type));

    /** @brief Insert a range of element by iterating over iterators */
    template<std::input_iterator InputIterator> requires std::constructible_from<Type, decltype(*std::declval<InputIterator>())>
    Iterator insert(const Iterator pos, const InputIterator from, const InputIterator to)
        noexcept(nothrow_forward_iterator_constructible(InputIterator) && nothrow_forward_constructible(Type) && nothrow_destructible(Type));


    /** @brief Remove a range of elements */
    void erase(const Iterator from, const Iterator to)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type));

    /** @brief Remove a range of elements */
    void erase(const Iterator from, const Range count)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { erase(from, from + count); }

    /** @brief Remove a specific element */
    void erase(const Iterator pos)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { erase(pos, pos + 1); }


    /** @brief Resize the vector using default constructor to initialize each element */
    void resize(const Range count)
        noexcept(std::is_nothrow_constructible_v<Type> && nothrow_destructible(Type));

    /** @brief Resize the vector by copying given element */
    void resize(const Range count, const Type &type)
        noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type));

    /** @brief Resize the vector with input iterators */
    template<std::input_iterator InputIterator>
    void resize(const InputIterator from, const InputIterator to)
        noexcept(nothrow_destructible(Type) && nothrow_forward_iterator_constructible(InputIterator));


    /** @brief Destroy all elements */
    void clear(void) noexcept_destructible(Type);
    void clearUnsafe(void) noexcept_destructible(Type);

    /** @brief Destroy all elements and release the buffer instance */
    void release(void) noexcept_destructible(Type);
    void releaseUnsafe(void) noexcept_destructible(Type);


    /** @brief Reserve memory for fast emplace only if asked capacity is higher than current capacity
     *  The data is either preserved or moved
     *  @return True if the reserve happened and the data has been moved */
    bool reserve(const Range capacity) noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type));


    /** @brief Grow internal buffer of a given minimum */
    void grow(const Range minimum = Range()) noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type));

private:
    /** @brief Reserve unsafe takes IsSafe as template parameter */
    template<bool IsSafe>
    bool reserveUnsafe(const Range capacity) noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type));
};

#include "VectorDetails.ipp"