/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A set of template utils used globally
 */

#pragma once

#include <type_traits>
#include <cinttypes>

/** @brief Fill a cacheline gap of a member variable if a condition is true */
#define KF_CACHELINE_ALIGNED_MEMBER_IF(condition, Type, name, ...) \
    [[no_unique_address]] std::conditional_t<condition, std::uint8_t[kF::Core::Utils::CacheLineSize - (sizeof(Type) % kF::Core::Utils::CacheLineSize)], kF::Core::Utils::Dummy> __padding##name; \
    Type name { __VA_ARGS__ }

/** @brief Same as KF_CACHELINE_ALIGNED_MEMBER_IF but always true */
#define KF_CACHELINE_ALIGNED_MEMBER(Type, name, ...) KF_FILL_CACHELINE_MEMBER_IF(true, Type, name, __VA_ARGS__)

namespace kF::Core::Utils
{
    /** @brief Helper used to use the right noexcept mode uppon forwarding constructor */
    template<typename Type, bool ForceCopy = false>
    constexpr bool NothrowCopyOrMoveConstruct = [] {
        if constexpr (!ForceCopy && std::is_move_constructible_v<Type>)
            return std::is_nothrow_move_constructible_v<Type> && std::is_nothrow_default_constructible_v<Type>;
        else
            return std::is_nothrow_copy_constructible_v<Type>;
    }();

    /** @brief Helper used to use the right noexcept mode uppon forwarding assignment */
    template<typename Type, bool ForceCopy = false>
    constexpr bool NothrowCopyOrMoveAssign = [] {
        if constexpr (!ForceCopy && std::is_move_assignable_v<Type>)
            return std::is_nothrow_move_assignable_v<Type> && std::is_nothrow_default_constructible_v<Type>;
        else
            return std::is_nothrow_copy_assignable_v<Type>;
    }();

    /** @brief Helper used to perfect forward move / copy constructor */
    template<typename Type, bool ForceCopy = false>
    void ForwardConstruct(Type *dest, Type *source) noexcept(NothrowCopyOrMoveConstruct<Type>) {
        if constexpr (!ForceCopy && std::is_move_assignable_v<Type>)
            new (dest) Type(std::move(*source));
        else
            new (dest) Type(*source);
    }

    /** @brief Helper used to perfect forward move / copy assignment */
    template<typename Type, bool ForceCopy = false>
    void ForwardAssign(Type *dest, Type *source) noexcept(NothrowCopyOrMoveAssign<Type>) {
        if constexpr (!ForceCopy && std::is_move_assignable_v<Type>)
            *dest = std::move(*source);
        else
            *dest = *source;
    }

    /** @brief Theorical cacheline size */
    constexpr std::size_t CacheLineSize = 64ul;

    /** @brief A dummy structure of null size */
    struct Dummy {};
}