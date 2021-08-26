/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SmallVectorBase
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core::Internal
{
    template<typename Type, std::size_t OptimizedCapacity, std::integral Range>
    class SmallVectorBase;
}

/** @brief Base implementation of a vector with size and capacity cached */
template<typename Type, std::size_t OptimizedCapacity, std::integral Range>
class kF::Core::Internal::SmallVectorBase
{
public:
    /** @brief Output iterator */
    using Iterator = Type *;

    /** @brief Input iterator */
    using ConstIterator = const Type *;


    /** @brief Always safe ! */
    [[nodiscard]] constexpr bool isSafe(void) const noexcept { return true; }

    /** @brief Fast empty check */
    [[nodiscard]] bool empty(void) const noexcept { return !_size; }


    /** @brief Get internal data pointer */
    [[nodiscard]] Type *data(void) noexcept { return dataUnsafe(); }
    [[nodiscard]] const Type *data(void) const noexcept { return dataUnsafe(); }
    [[nodiscard]] Type *dataUnsafe(void) noexcept { return _data; }
    [[nodiscard]] const Type *dataUnsafe(void) const noexcept { return _data; }


    /** @brief Get the size of the vector */
    [[nodiscard]] Range size(void) const noexcept { return sizeUnsafe(); }
    [[nodiscard]] Range sizeUnsafe(void) const noexcept { return _size; }


    /** @brief Get the capacity of the vector */
    [[nodiscard]] Range capacity(void) const noexcept { return capacityUnsafe(); }
    [[nodiscard]] Range capacityUnsafe(void) const noexcept { return _capacity; }


    /** @brief Unsafe begin / end overloads */
    [[nodiscard]] Iterator beginUnsafe(void) noexcept { return data(); }
    [[nodiscard]] Iterator endUnsafe(void) noexcept { return data() + sizeUnsafe(); }
    [[nodiscard]] ConstIterator beginUnsafe(void) const noexcept { return data(); }
    [[nodiscard]] ConstIterator endUnsafe(void) const noexcept { return data() + sizeUnsafe(); }

    /** @brief Begin / end overloads */
    [[nodiscard]] Iterator begin(void) noexcept { return beginUnsafe(); }
    [[nodiscard]] Iterator end(void) noexcept { return endUnsafe(); }
    [[nodiscard]] ConstIterator begin(void) const noexcept { return beginUnsafe(); }
    [[nodiscard]] ConstIterator end(void) const noexcept { return endUnsafe(); }


    /** @brief Steal another instance */
    void steal(SmallVectorBase &other) noexcept_forward_constructible(Type);

    /** @brief Swap two instances */
    void swap(SmallVectorBase &other) noexcept_forward_assignable(Type);


    /** @brief Tell if the vector currently use its cache or a heap allocation*/
    [[nodiscard]] bool isCacheUsed(void) const noexcept
        { return _data == optimizedData(); }

protected:
    /** @brief Protected data setter */
    void setData(Type * const data) noexcept { _data = data; }

    /** @brief Protected size setter */
    void setSize(const Range size) noexcept { _size = size; }

    /** @brief Protected capacity setter */
    void setCapacity(const Range capacity) noexcept { _capacity = capacity; }


    /** @brief Allocates a new buffer */
    [[nodiscard]] Type *allocate(const Range capacity) noexcept;

    /** @brief Deallocates a buffer */
    void deallocate(Type * const data, const Range capacity) noexcept;

    /** @brief Get a pointer to the data cache */
    [[nodiscard]] Type *optimizedData(void) noexcept
        { return reinterpret_cast<Type *>(&_optimizedData); }
    [[nodiscard]] const Type *optimizedData(void) const noexcept
        { return reinterpret_cast<const Type *>(&_optimizedData); }

private:
    alignas(alignof(Type)) std::byte _optimizedData[sizeof(Type) * OptimizedCapacity];
    Range _size {};
    Range _capacity {};
    Type *_data { nullptr };
};

#include "SmallVectorBase.ipp"