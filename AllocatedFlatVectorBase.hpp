/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatVector
 */

#pragma once

#include "FlatVectorBase.hpp"

namespace kF::Core::Internal
{
    template<typename Type, std::integral Range, auto AllocateFunc, auto DeallocateFunc>
    class AllocatedFlatVectorBase;
}

/** @brief Base implementation of a vector with size and capacity allocated with data */
template<typename Type, std::integral Range, auto AllocateFunc, auto DeallocateFunc>
class kF::Core::Internal::AllocatedFlatVectorBase : public FlatVectorBase<Type, Range>
{
protected:
    using Header = FlatVectorBase<Type, Range>::Header;

    /** @brief Allocates a new buffer */
    [[nodiscard]] Type *allocate(const Range capacity) noexcept
    {
        return reinterpret_cast<Type *>(
            reinterpret_cast<Header *>(
                AllocateFunc(sizeof(Header) + sizeof(Type) * capacity, alignof(Header))
            ) + 1
        );
    }

    /** @brief Deallocates a buffer */
    void deallocate(Type * const data, const Range capacity) noexcept
    {
        DeallocateFunc(
            reinterpret_cast<Header *>(data) - 1,
            sizeof(Header) + sizeof(Type) * capacity,
            alignof(Header)
        );
    }
};
