/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Small Vector
 */

#pragma once

#include "VectorDetails.hpp"

namespace kF::Core
{
    namespace Internal
    {
        template<typename Type, std::size_t OptimizedCapacity, std::integral Range>
        class SmallVectorBase;
    }

    template<typename Type, std::size_t OptimizedCapacity, std::integral Range = std::size_t>
    using SmallVector = Internal::VectorDetails<Internal::SmallVectorBase<Type, OptimizedCapacity, Range>, Type, Range, true>;

    template<typename Type, std::size_t OptimizedCapacity>
    using TinySmallVector = SmallVector<Type, OptimizedCapacity, std::uint32_t>;
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


    /** @brief Fast empty check */
    [[nodiscard]] bool empty(void) const noexcept { return !_size; }


    /** @brief Get internal data pointer */
    [[nodiscard]] Type *data(void) noexcept { return dataUnsafe(); }
    [[nodiscard]] const Type *data(void) const noexcept { return dataUnsafe(); }

    /** @brief Get the size of the vector */
    [[nodiscard]] Range size(void) const noexcept { return sizeUnsafe(); }

    /** @brief Get the capacity of the vector */
    [[nodiscard]] Range capacity(void) const noexcept { return capacityUnsafe(); }


    /** @brief Begin / end overloads */
    [[nodiscard]] Iterator begin(void) noexcept { return beginUnsafe(); }
    [[nodiscard]] Iterator end(void) noexcept { return endUnsafe(); }
    [[nodiscard]] ConstIterator begin(void) const noexcept { return beginUnsafe(); }
    [[nodiscard]] ConstIterator end(void) const noexcept { return endUnsafe(); }


    /** @brief Swap two instances */
    void swap(SmallVectorBase &other) noexcept;


    /** @brief Tell if the vector currently use its cache or a heap allocation*/
    [[nodiscard]] bool isCacheUsed(void) const noexcept
        { return _data == optimizedData(); }

protected:
    /** @brief Unsafe size getter */
    [[nodiscard]] Range sizeUnsafe(void) const noexcept { return _size; }

    /** @brief Unsafe capacity getter */
    [[nodiscard]] Range capacityUnsafe(void) const noexcept { return _capacity; }


    /** @brief Protected data setter */
    void setData(Type * const data) noexcept { _data = data; }

    /** @brief Protected size setter */
    void setSize(const Range size) noexcept { _size = size; }

    /** @brief Protected capacity setter */
    void setCapacity(const Range capacity) noexcept { _capacity = capacity; }

    /** @brief Unsafe data */
    [[nodiscard]] Type *dataUnsafe(void) noexcept { return _data; }
    [[nodiscard]] const Type *dataUnsafe(void) const noexcept { return _data; }

    /** @brief Unsafe begin / end overloads */
    [[nodiscard]] Iterator beginUnsafe(void) noexcept { return data(); }
    [[nodiscard]] Iterator endUnsafe(void) noexcept { return data() + sizeUnsafe(); }
    [[nodiscard]] ConstIterator beginUnsafe(void) const noexcept { return data(); }
    [[nodiscard]] ConstIterator endUnsafe(void) const noexcept { return data() + sizeUnsafe(); }


    /** @brief Allocates a new buffer */
    [[nodiscard]] Type *allocate(const Range capacity) noexcept;

    /** @brief Deallocates a buffer */
    void deallocate(Type *data) noexcept;


private:
    alignas(alignof(Type)) std::byte _optimizedData[sizeof(Type) * OptimizedCapacity];
    Type *_data { nullptr };
    Range _size {};
    Range _capacity {};

    [[nodiscard]] Type *optimizedData(void) noexcept
        { return reinterpret_cast<Type *>(&_optimizedData); }

    [[nodiscard]] const Type *optimizedData(void) const noexcept
        { return reinterpret_cast<const Type *>(&_optimizedData); }
};

#include "SmallVector.ipp"