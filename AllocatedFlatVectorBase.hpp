/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

#pragma once

#include "FlatVectorBase.hpp"

namespace kF::Core::Internal
{
    template<typename Type, std::integral Range, auto AllocateFunc, auto DeallocateFunc, typename CustomHeaderType>
    class AllocatedFlatVectorBase;
}

/** @brief Base implementation of a vector with size and capacity allocated with data */
template<typename Type, std::integral Range, auto AllocateFunc, auto DeallocateFunc, typename CustomHeaderType>
class kF::Core::Internal::AllocatedFlatVectorBase : public FlatVectorBase<Type, Range, CustomHeaderType>
{
protected:
    using Header = FlatVectorBase<Type, Range, CustomHeaderType>::Header;

    /** @brief Allocates a new buffer */
    [[nodiscard]] Type *allocate(const Range capacity) noexcept
    {
        auto ptr = reinterpret_cast<Header *>(AllocateFunc(sizeof(Header) + sizeof(Type) * capacity, alignof(Header)));
        if constexpr (!std::is_same_v<CustomHeaderType, Internal::NoCustomHeaderType>)
            new (&ptr->customType) CustomHeaderType {};
        return reinterpret_cast<Type *>(ptr + 1);
    }

    /** @brief Deallocates a buffer */
    void deallocate(Type * const data, const Range capacity) noexcept
    {
        auto ptr = reinterpret_cast<Header *>(data) - 1;
        if constexpr (!std::is_same_v<CustomHeaderType, Internal::NoCustomHeaderType>)
            ptr->customType.~CustomHeaderType();
        DeallocateFunc(ptr, sizeof(Header) + sizeof(Type) * capacity, alignof(Header));
    }
};
