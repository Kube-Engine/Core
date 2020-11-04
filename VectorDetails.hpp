/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: VectorDetails
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core::Internal
{
    // template<typename Base, typename Type, typename Range>
    // concept VectorBase = std::integral<Range> &&
    //     requires(Base &base) {
    //         std::same_as<Type *, base.data()>;
    //     } && requires(const Base &base) {
    //         std::same_as<const Type *, base.data()>;
    //     };

    template<typename Base, typename Type, std::integral Range> //requires VectorBase<Base, Type, Range>
    class VectorDetails;
}

template<typename Base, typename Type, std::integral Range>// requires kF::Core::Internal::VectorBase<Base, Type, Range>
class kF::Core::Internal::VectorDetails : public Base
{
public:
    /** @brief Iterator detectors */
    using Iterator = decltype(std::declval<Base &>().begin());
    using ConstIterator = decltype(std::declval<const Base &>().begin());

    /** @brief Default constructor */
    VectorDetails(void) noexcept = default;

    /** @brief Release the vector */
    ~VectorDetails(void) noexcept_destructible(Type) { release(); }


    /** @brief Begin / End helpers */
    [[nodiscard]] ConstIterator cbegin(void) const noexcept { return begin(); }
    [[nodiscard]] ConstIterator cend(void) const noexcept { return end(); }


    /** @brief Access element at positon */
    [[nodiscard]] Type &at(const Range pos ) noexcept { return data()[pos]; }
    [[nodiscard]] const Type &at(const Range pos) const noexcept { return data()[pos]; }

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


    /** @brief Reserve memory for fast emplace */
    void reserve(const Range capacity) noexcept_destructible(Type);


    /** @brief Destroy all elements */
    void clear(void) noexcept_destructible(Type);
    void clearUnsafe(void) noexcept_destructible(Type);

    /** @brief Destroy all elements and release the buffer instance */
    void release(void) noexcept_destructible(Type);
    void releaseUnsafe(void) noexcept_destructible(Type);


    /** @brief Grow internal buffer */
    void grow(const Range minimum = Range()) noexcept(nothrow_forward_constructible(Type) && nothrow_destructible(Type));
};

#include "VectorDetails.ipp"