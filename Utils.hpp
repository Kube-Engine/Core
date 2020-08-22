/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A set of template utils used globally
 */

#pragma once

#include <type_traits>
#include <cinttypes>

#define KF_ALIGN_CACHELINE alignas(kF::Core::Utils::CacheLineSize)
#define KF_ALIGN_CACHELINE2 alignas(kF::Core::Utils::CacheLineSize * 2)

/** @brief Compile-time ternary expression */
#define ConstexprTernary(condition, body, elseBody) [] { if constexpr (condition) { return body; } else { return elseBody; } }()

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