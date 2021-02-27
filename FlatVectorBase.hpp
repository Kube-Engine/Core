/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

#pragma once

#include "Utils.hpp"

namespace kF::Core::Internal
{
    template<typename Type, std::integral Range, typename CustomHeaderType>
    class FlatVectorBase;

    /** @brief Deduce the alignment of FlatVectorHeader */
    template<typename Type, typename Range, std::size_t CustomHeaderTypeSize>
    [[nodiscard]] constexpr std::size_t GetFlatVectorHeaderAlignment(void)
    {
        constexpr std::size_t TotalHeaderSize = sizeof(Range) * 2 + CustomHeaderTypeSize;
        constexpr std::size_t MinAlignment = alignof(Type);

        if constexpr (TotalHeaderSize > MinAlignment)
            return kF::Core::Utils::NextPowerOf2(TotalHeaderSize);
        else
            return MinAlignment;
    }

    /** @brief Header of the FlatVector with custom type */
    template<typename Type, typename Range, typename CustomHeaderType>
    struct alignas(GetFlatVectorHeaderAlignment<Type, Range, sizeof(CustomHeaderType)>()) FlatVectorHeader
    {
        CustomHeaderType customType {};
        Range size {};
        Range capacity {};
    };

    /** @brief Header of the FlatVector without custom type */
    template<typename Type, typename Range>
    struct alignas(GetFlatVectorHeaderAlignment<Type, Range, 0>()) FlatVectorHeader<Type, Range, void>
    {
        Range size {};
        Range capacity {};
    };

    // static_assert_fit_cacheline(TEMPLATE_TYPE(FlatVectorHeader, std::size_t, std::size_t, char[48]));
    // static_assert_fit_eighth_cacheline(TEMPLATE_TYPE(FlatVectorHeader, std::size_t, std::uint32_t, void));
}

/** @brief Base implementation of a vector with size and capacity allocated with data */
template<typename Type, std::integral Range, typename CustomHeaderType>
class kF::Core::Internal::FlatVectorBase
{
public:
    /** @brief Output iterator */
    using Iterator = Type *;

    /** @brief Input iterator */
    using ConstIterator = const Type *;

    /** @brief FlatVector's header */
    using Header = FlatVectorHeader<Type, Range, CustomHeaderType>;


    /** @brief Check if the instance is safe to access */
    [[nodiscard]] bool isSafe(void) const noexcept { return _ptr; }

    /** @brief Fast empty check */
    [[nodiscard]] bool empty(void) const noexcept { return !_ptr || !sizeUnsafe(); }


    /** @brief Get internal data pointer */
    [[nodiscard]] Type *data(void) noexcept
        { return const_cast<Type *>(const_cast<const FlatVectorBase *>(this)->data()); }
    [[nodiscard]] const Type *data(void) const noexcept;
    [[nodiscard]] Type *dataUnsafe(void) noexcept { return reinterpret_cast<Type *>(_ptr + 1); }
    [[nodiscard]] const Type *dataUnsafe(void) const noexcept { return reinterpret_cast<const Type *>(_ptr + 1); }

    /** @brief Get the size of the vector */
    [[nodiscard]] Range size(void) const noexcept { return _ptr ? sizeUnsafe() : Range(); }
    [[nodiscard]] Range sizeUnsafe(void) const noexcept { return _ptr->size; }

    /** @brief Get the capacity of the vector */
    [[nodiscard]] Range capacity(void) const noexcept { return _ptr ? capacityUnsafe() : Range(); }
    [[nodiscard]] Range capacityUnsafe(void) const noexcept { return _ptr->capacity; }


    /** @brief Begin / end overloads */
    [[nodiscard]] Iterator begin(void) noexcept { return _ptr ? beginUnsafe() : Iterator(); }
    [[nodiscard]] Iterator end(void) noexcept { return _ptr ? endUnsafe() : Iterator(); }
    [[nodiscard]] ConstIterator begin(void) const noexcept { return _ptr ? beginUnsafe() : ConstIterator(); }
    [[nodiscard]] ConstIterator end(void) const noexcept { return _ptr ? endUnsafe() : ConstIterator(); }

    /** @brief Unsafe begin / end overloads */
    [[nodiscard]] Iterator beginUnsafe(void) noexcept { return data(); }
    [[nodiscard]] Iterator endUnsafe(void) noexcept { return data() + sizeUnsafe(); }
    [[nodiscard]] ConstIterator beginUnsafe(void) const noexcept { return data(); }
    [[nodiscard]] ConstIterator endUnsafe(void) const noexcept { return data() + sizeUnsafe(); }

    /** @brief Steal another instance */
    void steal(FlatVectorBase &other) noexcept;

    /** @brief Swap two instances */
    void swap(FlatVectorBase &other) noexcept { std::swap(_ptr, other._ptr); }

    /** @brief Get the custom type in header if any (doesn't check if the vector is allocated) */
    template<typename As = CustomHeaderType>
    [[nodiscard]] std::enable_if_t<!std::is_same_v<As, void>, int> headerCustomType(void) noexcept
        { return /*_ptr->customType*/ 42; }

    template<typename As = CustomHeaderType>
    [[nodiscard]] std::enable_if_t<!std::is_same_v<As, void>, int> headerCustomType(void) const noexcept
        { return /*_ptr->customType*/ 42; }

protected:
    /** @brief Protected data setter */
    void setData(Type * const data) noexcept { _ptr = reinterpret_cast<Header *>(data) - 1; }

    /** @brief Protected size setter */
    void setSize(const Range size) noexcept { _ptr->size = size; }

    /** @brief Protected capacity setter */
    void setCapacity(const Range capacity) noexcept { _ptr->capacity = capacity; }


    /** @brief Allocates a new buffer */
    [[nodiscard]] Type *allocate(const Range capacity) noexcept
    {
        auto ptr = Utils::AlignedAlloc<alignof(Header), Header>(sizeof(Header) + sizeof(Type) * capacity);
        if constexpr (!std::is_same_v<CustomHeaderType, void>)
            new (&ptr->customType) CustomHeaderType {};
        return reinterpret_cast<Type *>(ptr + 1);
    }

    /** @brief Deallocates a buffer */
    void deallocate(Type * const data, const Range) noexcept
    {
        auto ptr = reinterpret_cast<Header *>(data) - 1;
        if constexpr (!std::is_same_v<CustomHeaderType, void>)
            ptr->customType.~CustomHeaderType();
        Utils::AlignedFree(ptr);
    }

private:
    Header *_ptr { nullptr };
};

#include "FlatVectorBase.ipp"