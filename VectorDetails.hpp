/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: VectorDetails
 */

#pragma once

#include <algorithm>
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
    using Base::steal;
    using Base::swap;

    /** @brief Default constructor */
    VectorDetails(void) noexcept = default;

    /** @brief Copy constructor */
    VectorDetails(const VectorDetails &other) noexcept_copy_constructible(Type)
        { resize(other.begin(), other.end()); }

    /** @brief Move constructor */
    VectorDetails(VectorDetails &&other) noexcept { steal(other); }

    /** @brief Resize with default constructor */
    VectorDetails(const Range count)
        noexcept(nothrow_default_constructible(Type) && nothrow_destructible(Type))
        { resize(count); }

    /** @brief Resize with copy constructor */
    VectorDetails(const Range count, const Type &value)
        noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type))
        requires std::copy_constructible<Type>
        { resize(count, value); }

    /** @brief Resize constructor */
    template<std::input_iterator InputIterator>
    VectorDetails(InputIterator from, InputIterator to)
        noexcept(nothrow_forward_iterator_constructible(InputIterator) && nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { resize(from, to); }

    /** @brief Resize map constructor */
    template<std::input_iterator InputIterator, typename Map>
    VectorDetails(InputIterator from, InputIterator to, Map &&map)
        { resize(from, to, std::forward<Map>(map)); }

    /** @brief Initializer list constructor */
    VectorDetails(std::initializer_list<Type> &&init) noexcept_forward_constructible(Type)
        : VectorDetails(init.begin(), init.end()) {}

    /** @brief Release the vector */
    ~VectorDetails(void) noexcept_destructible(Type) { release(); }

    /** @brief Copy assignment */
    VectorDetails &operator=(const VectorDetails &other) noexcept_copy_constructible(Type)
        { resize(other.begin(), other.end()); return *this; }

    /** @brief Move assignment */
    VectorDetails &operator=(VectorDetails &&other) noexcept { steal(other); return *this; }

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
    template<typename ...Args> requires std::constructible_from<Type, Args...>
    Type &push(Args &&...args)
        noexcept(nothrow_constructible(Type, Args...) && nothrow_forward_constructible(Type) && nothrow_destructible(Type));

    /** @brief Pop the last element of the vector */
    void pop(void) noexcept_destructible(Type);


    /** @brief Insert a range of copies */
    Iterator insertDefault(Iterator pos, const Range count)
        noexcept(nothrow_default_constructible(Type) && nothrow_forward_constructible(Type) && nothrow_destructible(Type));

    /** @brief Insert a range of copies */
    Iterator insertCopy(Iterator pos, const Range count, const Type &value)
        noexcept(nothrow_copy_constructible(Type) && nothrow_forward_constructible(Type) && nothrow_destructible(Type));

    /** @brief Insert a value by copy */
    Iterator insert(Iterator pos, const Type &value)
        noexcept(nothrow_copy_constructible(Type) && nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { return insert(pos, &value, &value + 1); }

    /** @brief Insert a value by move */
    Iterator insert(Iterator pos, Type &&value)
        noexcept(nothrow_move_constructible(Type) && nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { return insert(pos, std::make_move_iterator(&value), std::make_move_iterator(&value + 1)); }

    /** @brief Insert an initializer list */
    Iterator insert(Iterator pos, std::initializer_list<Type> &&init)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { return insert(pos, init.begin(), init.end()); }

    /** @brief Insert a range of element by iterating over iterators */
    template<std::input_iterator InputIterator>
    Iterator insert(Iterator pos, InputIterator from, InputIterator to)
        noexcept(nothrow_forward_iterator_constructible(InputIterator) && nothrow_forward_constructible(Type) && nothrow_destructible(Type));

    /** @brief Insert a range of element by using a map function over iterators */
    template<std::input_iterator InputIterator, typename Map>
    Iterator insert(Iterator pos, InputIterator from, InputIterator to, Map &&map);


    /** @brief Remove a range of elements */
    void erase(Iterator from, Iterator to)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type));

    /** @brief Remove a range of elements */
    void erase(Iterator from, const Range count)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { erase(from, from + count); }

    /** @brief Remove a specific element */
    void erase(Iterator pos)
        noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { erase(pos, pos + 1); }


    /** @brief Resize the vector using default constructor to initialize each element */
    void resize(const Range count)
        noexcept(nothrow_default_constructible(Type) && nothrow_destructible(Type))
        requires std::constructible_from<Type>;

    /** @brief Resize the vector by copying given element */
    void resize(const Range count, const Type &value)
        noexcept(nothrow_copy_constructible(Type) && nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        requires std::copy_constructible<Type>;

    /** @brief Resize the vector with input iterators */
    template<std::input_iterator InputIterator>
    void resize(InputIterator from, InputIterator to)
        noexcept(nothrow_forward_iterator_constructible(InputIterator) && nothrow_forward_constructible(Type) && nothrow_destructible(Type));

    /** @brief Resize the vector using a map function with input iterators */
    template<std::input_iterator InputIterator, typename Map>
    void resize(InputIterator from, InputIterator to, Map &&map);


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


    /** @brief Comparison operators */
    [[nodiscard]] bool operator==(const VectorDetails &other) const noexcept
        requires std::equality_comparable<Type>;
    [[nodiscard]] bool operator!=(const VectorDetails &other) const noexcept
        requires std::equality_comparable<Type>
        { return !operator==(other); }

    /** @brief Find an element by comparison */
    template<typename Comparable>
        requires std::equality_comparable_with<Comparable, Type>
    [[nodiscard]] Iterator find(const Comparable &comparable) noexcept
        { return std::find(begin(), end(), comparable); }
    template<typename Comparable>
        requires std::equality_comparable_with<Comparable, Type>
    [[nodiscard]] ConstIterator find(const Comparable &comparable) const noexcept
        { return std::find(begin(), end(), comparable); }

    /** @brief Find an element with functor */
    template<typename Functor>
        requires std::invocable<Functor, Type &>
    [[nodiscard]] Iterator find(Functor &&functor) noexcept
        { return std::find_if(begin(), end(), std::forward<Functor>(functor)); }
    template<typename Functor>
        requires std::invocable<Functor, const Type &>
    [[nodiscard]] ConstIterator find(Functor &&functor) const noexcept
        { return std::find_if(begin(), end(), std::forward<Functor>(functor)); }


    /** @brief Grow internal buffer of a given minimum */
    void grow(const Range minimum = Range()) noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type));

protected:
    /** @brief Reserve unsafe takes IsSafe as template parameter */
    template<bool IsSafe = true>
    bool reserveUnsafe(const Range capacity) noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type));
};

#include "VectorDetails.ipp"