/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A set of utility used globally in the framework
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

    /** @brief Helper used to construct 'output' by forwarding 'input' */
    template<typename Type, bool ForceCopy = false, bool DestroyInput = false>
    void ForwardConstruct(Type *output, Type *input) noexcept(NothrowCopyOrMoveConstruct<Type>) {
        if constexpr (!ForceCopy && std::is_move_assignable_v<Type>)
            new (output) Type(std::move(*input));
        else
            new (output) Type(*input);
        if constexpr (DestroyInput)
            input->~Type();
    }

    /** @brief Helper used to assign 'output' by forwarding 'input' */
    template<typename Type, bool ForceCopy = false, bool DestroyInput>
    void ForwardAssign(Type *output, Type *input) noexcept(NothrowCopyOrMoveAssign<Type>) {
        if constexpr (!ForceCopy && std::is_move_assignable_v<Type>)
            *output = std::move(*input);
        else
            *output = *input;
        if constexpr (DestroyInput)
            input->~Type();

    }

    /** @brief Helper used to construct a range of 'output' by forwarding 'inputs' */
    template<typename Type, bool ForceCopy = false, bool DestroyInput = false>
    void ForwardConstructRange(Type *outputs, Type *inputs, const std::size_t count) {
        if constexpr (std::is_trivially_copyable_v<Type>)
            std::copy_n(inputs, count, outputs);
        else {
            for (auto i = 0ul; i < count; ++i)
                ForwardConstruct<Type, ForceCopy, DestroyInput>(inputs + i, outputs + i);
        }
    }

    /** @brief Helper used to assign a range of 'outputs' by forwarding 'inputs' */
    template<typename Type, bool ForceCopy = false, bool DestroyInput = false>
    void ForwardAssignRange(Type *outputs, Type *inputs, const std::size_t count) {
        if constexpr (std::is_trivially_copyable_v<Type>)
            std::copy_n(inputs, count, outputs);
        else {
            for (auto i = 0ul; i < count; ++i)
                ForwardAssign<Type, ForceCopy, DestroyInput>(inputs + i, outputs + i);
        }
    }

    /** @brief Theorical cacheline size */
    constexpr std::size_t CacheLineSize = 64ul;

    /** @brief A dummy structure of null size */
    struct Dummy {};
}