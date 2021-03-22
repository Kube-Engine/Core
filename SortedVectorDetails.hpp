/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedVectorDetails
 */

#pragma once


#include "VectorDetails.hpp"

namespace kF::Core::Internal
{
    template<typename Base, typename Type, std::integral Range, typename Compare, bool IsSmallOptimized = false>
    class SortedVectorDetails;
}

template<typename Base, typename Type, std::integral Range, typename Compare, bool IsSmallOptimized>
class kF::Core::Internal::SortedVectorDetails : public VectorDetails<Base, Type, Range, IsSmallOptimized>
{
public:
    /** @brief Type alias to VectorDetails */
    using DetailsBase = VectorDetails<Base, Type, Range, IsSmallOptimized>;

    /** @brief Iterator detectors */
    using Iterator = typename DetailsBase::Iterator;
    using ConstIterator = typename DetailsBase::ConstIterator;

    /** @brief Default constructor */
    SortedVectorDetails(void) noexcept = default;

    /** @brief Copy constructor */
    SortedVectorDetails(const SortedVectorDetails &other) noexcept_copy_constructible(Type)
        { DetailsBase::resize(other.begin(), other.end()); }

    /** @brief Move constructor */
    SortedVectorDetails(SortedVectorDetails &&other) noexcept { DetailsBase::steal(other); }

    /** @brief Resize with default constructor */
    SortedVectorDetails(const Range count)
        noexcept(nothrow_default_constructible(Type) && nothrow_destructible(Type))
        { resize(count); }

    /** @brief Resize with copy constructor */
    SortedVectorDetails(const Range count, const Type &value)
        noexcept(nothrow_copy_constructible(Type) && nothrow_destructible(Type))
        requires std::copy_constructible<Type>
        { resize(count, value); }

    /** @brief Resize constructor */
    template<std::input_iterator InputIterator>
    SortedVectorDetails(InputIterator from, InputIterator to)
        noexcept(nothrow_forward_iterator_constructible(InputIterator) && nothrow_forward_constructible(Type) && nothrow_destructible(Type))
        { resize(from, to); }

    /** @brief Resize map constructor */
    template<std::input_iterator InputIterator, typename Map>
    SortedVectorDetails(InputIterator from, InputIterator to, Map &&map)
        { resize(from, to, std::forward<Map>(map)); }

    /** @brief Initializer list constructor */
    SortedVectorDetails(std::initializer_list<Type> &&init) noexcept_forward_constructible(Type)
        : SortedVectorDetails(init.begin(), init.end()) {}

    /** @brief Release the vector */
    ~SortedVectorDetails(void) noexcept_destructible(Type) = default;

    /** @brief Copy assignment */
    SortedVectorDetails &operator=(const SortedVectorDetails &other) noexcept_copy_constructible(Type)
        { DetailsBase::resize(other.begin(), other.end()); return *this; }

    /** @brief Move assignment */
    SortedVectorDetails &operator=(SortedVectorDetails &&other) noexcept
        { DetailsBase::steal(other); return *this; }

    /** @brief Push an element into the vector */
    template<typename ...Args> requires std::constructible_from<Type, Args...>
    Type &push(Args &&...args);


    /** @brief Insert a range of default initialized values */
    void insertDefault(const Range count)
        noexcept(nothrow_default_constructible(Type) && nothrow_destructible(Type));

    /** @brief Insert a range of copies */
    void insertCopy(const Range count, const Type &value);

    /** @brief Insert an initializer list */
    void insert(std::initializer_list<Type> &&init)
        { return insert(init.begin(), init.end()); }

    /** @brief Insert an value by copy, returning its iterator */
    Iterator insert(const Type &value)
        { return &push(value); }

    /** @brief Insert an value by move, returning its iterator */
    Iterator insert(Type &&value)
        { return &push(std::move(value)); }

    /** @brief Insert a range of element by iterating over iterators */
    template<std::input_iterator InputIterator>
    void insert(InputIterator from, InputIterator to);

    /** @brief Insert a range of element by using a map function over iterators */
    template<std::input_iterator InputIterator, typename Map>
    void insert(InputIterator from, InputIterator to, Map &&map);


    /** @brief Resize the vector using default constructor to initialize each element */
    void resize(const Range count)
        noexcept(nothrow_destructible(Type) && nothrow_default_constructible(Type))
        requires std::constructible_from<Type>
        { DetailsBase::resize(count); }

    /** @brief Resize the vector by copying given element */
    void resize(const Range count, const Type &type)
        noexcept(nothrow_destructible(Type) && nothrow_copy_constructible(Type))
        requires std::copy_constructible<Type>
        { DetailsBase::resize(count, type); }

    /** @brief Resize the vector with input iterators */
    template<std::input_iterator InputIterator>
    void resize(InputIterator from, InputIterator to);

    /** @brief Resize the vector using a map function with input iterators */
    template<std::input_iterator InputIterator, typename Map>
    void resize(InputIterator from, InputIterator to, Map &&map);

    /** @brief Sort the vector */
    void sort(void);

    /** @brief Assign a new value to an existing element
     *  @return The index where the element has been moved if assignment break sort */
    template<typename AssignType>
    Range assign(const Range index, AssignType &&value);


    /** @brief Finds where to insert an element */
    [[nodiscard]] Iterator findSortedPlacement(const Type &value)
        noexcept_invocable(Compare, const Type &, const Type &)
        { return DetailsBase::find([&value](const Type &other) { return Compare{}(value, other); }); }
    [[nodiscard]] ConstIterator findSortedPlacement(const Type &value) const
        noexcept_invocable(Compare, const Type &, const Type &)
        { return DetailsBase::find([&value](const Type &other) { return Compare{}(value, other); }); }

private:
    /** @brief Reimplemented functions */
    using DetailsBase::push;
    using DetailsBase::insert;
    using DetailsBase::resize;
};

#include "SortedVectorDetails.ipp"