/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Base data of a vector
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core::Internal
{
    template<typename Type, std::integral Range>
    class VectorBase;
}

/** @brief Base implementation of a vector with size and capacity cached */
template<typename Type, std::integral Range>
class kF::Core::Internal::VectorBase
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

    /** @brief Steal another instance */
    void steal(VectorBase &other) noexcept;

    /** @brief Swap two instances */
    void swap(VectorBase &other) noexcept;

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
    [[nodiscard]] Type *allocate(const Range capacity) noexcept
        { return reinterpret_cast<Type *>(Utils::AlignedAlloc<alignof(Type)>(sizeof(Type) * capacity)); }

    /** @brief Deallocates a buffer */
    void deallocate(Type * const data, const Range) noexcept { Utils::AlignedFree(data); }

private:
    Type *_data { nullptr };
    Range _size {};
    Range _capacity {};
};

#include "VectorBase.ipp"