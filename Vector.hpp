/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Vector
 */

#pragma once

#include "VectorDetails.hpp"

namespace kF::Core
{
    namespace Internal
    {
        template<typename Type, std::integral Range>
        class VectorBase;
    }

    template<typename Type, std::integral Range = std::size_t>
    using Vector = Internal::VectorDetails<Internal::VectorBase<Type, Range>, Type, Range>;
}

template<typename Type, std::integral Range>
class kF::Core::Internal::VectorBase
{
public:
    using Iterator = Type *;
    using ConstIterator = const Type *;

    /** @brief Fast empty check */
    [[nodiscard]] bool empty(void) const noexcept { return !_size; }


    /** @brief Get internal data pointer */
    [[nodiscard]] Type *data(void) noexcept { return _data; }
    [[nodiscard]] const Type *data(void) const noexcept { return _data; }

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
    void swap(VectorBase &other) noexcept;

protected:

    /** @brief Check if the vector is safe to access */
    [[nodiscard]] constexpr bool isSafe(void) const noexcept { return true; }

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


    /** @brief Unsafe begin / end overloads */
    [[nodiscard]] Iterator beginUnsafe(void) noexcept { return _data; }
    [[nodiscard]] Iterator endUnsafe(void) noexcept { return endUnsafe(); }
    [[nodiscard]] ConstIterator beginUnsafe(void) const noexcept { return _data; }
    [[nodiscard]] ConstIterator endUnsafe(void) const noexcept { return endUnsafe(); }


    /** @brief Allocates a new buffer */
    [[nodiscard]] Type *allocate(const Range capacity) noexcept
        { return reinterpret_cast<Type *>(std::malloc(sizeof(Type) * capacity)); }

    /** @brief Deallocates a buffer */
    [[nodiscard]] void deallocate(Type *data) noexcept { std::free(data); }

private:
    Type *_data { nullptr };
    Range _size {};
    Range _capacity {};
};

#include "Vector.ipp"