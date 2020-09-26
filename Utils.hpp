/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A set of utility used globally in the framework
 */

#pragma once

#include <type_traits>
#include <cinttypes>
#include <concepts>

/** @brief Various exception helpers */
#define nothrow_constructible(Type, ...) std::is_nothrow_constructible_v<Type __VA_OPT__(,) __VA_ARGS__>
#define nothrow_copy_constructible(Type) std::is_nothrow_copy_constructible_v<Type>
#define nothrow_move_constructible(Type) std::is_nothrow_move_constructible_v<Type>
#define nothrow_forward_constructible(Type) (std::is_move_constructible_v<Type> ? nothrow_move_constructible(Type) : nothrow_copy_constructible(Type))
#define nothrow_copy_assignable(Type) std::is_nothrow_copy_assignable_v<Type>
#define nothrow_move_assignable(Type) std::is_nothrow_move_assignable_v<Type>
#define nothrow_forward_assignable(Type) (std::is_move_assignable_v<Type> ? nothrow_move_assignable(Type) : nothrow_copy_assignable(Type))
#define nothrow_destructible(Type) std::is_nothrow_destructible_v<Type>
#define nothrow_invokable(Function, ...) std::is_nothrow_invokable_v<Function __VA_OPT__(,) __VA_ARGS__>
#define nothrow_forward_iterator_constructible(Type) (std::__is_move_iterator<Type>::__value ? nothrow_move_constructible(Type) : nothrow_copy_constructible(Type))
#define nothrow_expr(Expression) noexcept(Expression)

/** @brief Various noexcept helpers */
#define noexcept_constructible(Type, ...) noexcept(nothrow_constructible(Type __VA_OPT__(,) __VA_ARGS__))
#define noexcept_copy_constructible(Type) noexcept(nothrow_copy_constructible(Type))
#define noexcept_move_constructible(Type) noexcept(nothrow_move_constructible(Type))
#define noexcept_forward_constructible(Type) noexcept(nothrow_forward_constructible(Type))
#define noexcept_copy_assignable(Type) noexcept(nothrow_copy_assignable(Type))
#define noexcept_move_assignable(Type) noexcept(nothrow_move_assignable(Type))
#define noexcept_forward_assignable(Type) noexcept(nothrow_forward_assignable(Type))
#define noexcept_destructible(Type) noexcept(nothrow_destructible(Type))
#define noexcept_invokable(Function, ...) noexcept(nothrow_invokable(Function __VA_OPT__(,) __VA_ARGS__))
#define noexcept_expr(Expression) noexcept(nothrow_expr(Expression))

/** @brief Align a variable / structure to cacheline size */
#define KF_ALIGN_CACHELINE alignas(kF::Core::Utils::CacheLineSize)
#define KF_ALIGN_CACHELINE2 alignas(kF::Core::Utils::CacheLineSize * 2)

/** @brief Compile-time ternary expression */
#define ConstexprTernary(condition, body, elseBody) [] { if constexpr (condition) { return body; } else { return elseBody; } }()

namespace kF::Core::Utils
{
    /** @brief Helper used to construct 'output' by forwarding 'input' */
    template<typename Type, bool ForceCopy = false, bool DestructInput = false>
    void ForwardConstruct(Type *output, Type *input) noexcept((!DestructInput || nothrow_destructible(Type)) && ((!ForceCopy || std::is_move_constructible_v<Type>) ? nothrow_move_constructible(Type) : nothrow_copy_constructible(Type)))
    {
        if constexpr (!ForceCopy && std::is_move_assignable_v<Type>)
            new (output) Type(std::move(*input));
        else
            new (output) Type(*input);
        if constexpr (DestructInput)
            input->~Type();
    }

    /** @brief Helper used to assign 'output' by forwarding 'input' */
    template<typename Type, bool ForceCopy = false, bool DestructInput>
    void ForwardAssign(Type *output, Type *input) noexcept((!DestructInput || nothrow_destructible(Type)) && ((!ForceCopy || std::is_move_assignable_v<Type>) ? nothrow_move_assignable(Type) : nothrow_copy_assignable(Type)))
    {
        if constexpr (!ForceCopy && std::is_move_assignable_v<Type>)
            *output = std::move(*input);
        else
            *output = *input;
        if constexpr (DestructInput)
            input->~Type();

    }

    /** @brief Helper used to construct a range of 'output' by forwarding 'inputs' */
    template<typename Type, bool ForceCopy = false, bool DestructInput = false>
    void ForwardConstructRange(Type *outputs, Type *inputs, const std::size_t count) noexcept((!DestructInput || nothrow_destructible(Type)) && ((!ForceCopy || std::is_move_constructible_v<Type>) ? nothrow_move_constructible(Type) : nothrow_copy_constructible(Type)))
    {
        if constexpr (std::is_trivially_copyable_v<Type>)
            std::copy_n(inputs, count, outputs);
        else {
            for (auto i = 0ul; i < count; ++i)
                ForwardConstruct<Type, ForceCopy, DestructInput>(inputs + i, outputs + i);
        }
    }

    /** @brief Helper used to assign a range of 'outputs' by forwarding 'inputs' */
    template<typename Type, bool ForceCopy = false, bool DestructInput = false>
    void ForwardAssignRange(Type *outputs, Type *inputs, const std::size_t count) noexcept((!DestructInput || nothrow_destructible(Type)) && ((!ForceCopy || std::is_move_assignable_v<Type>) ? nothrow_move_assignable(Type) : nothrow_copy_assignable(Type)))
    {
        if constexpr (std::is_trivially_copyable_v<Type>)
            std::copy_n(inputs, count, outputs);
        else {
            for (auto i = 0ul; i < count; ++i)
                ForwardAssign<Type, ForceCopy, DestructInput>(inputs + i, outputs + i);
        }
    }

    /** @brief Theorical cacheline size */
    constexpr std::size_t CacheLineSize = 64ul;

    /** @brief A dummy structure of null size */
    struct Dummy {};
}