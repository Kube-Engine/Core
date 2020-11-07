/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A set of utility used globally in the framework
 */

#pragma once

#include <type_traits>
#include <cinttypes>
#include <concepts>
#include <cstddef>

/** @brief Various exception helpers */
#define nothrow_constructible(Type, ...) std::is_nothrow_constructible_v<Type __VA_OPT__(,) __VA_ARGS__>
#define nothrow_copy_constructible(Type) std::is_nothrow_copy_constructible_v<Type>
#define nothrow_move_constructible(Type) std::is_nothrow_move_constructible_v<Type>
#define nothrow_forward_constructible(Type) (std::is_move_constructible_v<Type> ? nothrow_move_constructible(Type) : nothrow_copy_constructible(Type))
#define nothrow_copy_assignable(Type) std::is_nothrow_copy_assignable_v<Type>
#define nothrow_move_assignable(Type) std::is_nothrow_move_assignable_v<Type>
#define nothrow_forward_assignable(Type) (std::is_move_assignable_v<Type> ? nothrow_move_assignable(Type) : nothrow_copy_assignable(Type))
#define nothrow_destructible(Type) std::is_nothrow_destructible_v<Type>
#define nothrow_invocable(Function, ...) std::is_nothrow_invocable_v<Function __VA_OPT__(,) __VA_ARGS__>
#define nothrow_forward_iterator_constructible(Type) (kF::Core::Utils::IsMoveIterator<Type>::Value ? nothrow_move_constructible(Type) : nothrow_copy_constructible(Type))
#define nothrow_convertible(From, To) std::is_nothrow_convertible_v<From, To>
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
#define noexcept_invocable(Function, ...) noexcept(nothrow_invocable(Function __VA_OPT__(,) __VA_ARGS__))
#define noexcept_convertible(From, To) noexcept(nothrow_convertible(From, To))
#define noexcept_expr(Expression) noexcept(nothrow_expr(Expression))

/** @brief Align a variable / structure to cacheline size */
#define KF_ALIGN_CACHELINE alignas(kF::Core::CacheLineSize)
#define KF_ALIGN_DOUBLE_CACHELINE alignas(kF::Core::CacheLineDoubleSize)
#define KF_ALIGN_HALF_CACHELINE alignas(kF::Core::CacheLineHalfSize)
#define KF_ALIGN_QUARTER_CACHELINE alignas(kF::Core::CacheLineQuarterSize)
#define KF_ALIGN_EIGHTH_CACHELINE alignas(kF::Core::CacheLineEighthSize)

/** @brief Compile-time ternary expression */
#define ConstexprTernary(condition, body, elseBody) [] { if constexpr (condition) { return body; } else { return elseBody; } }()

/** @brief Compile-time ternary expression with runtime reference capture */
#define ConstexprTernaryRef(condition, body, elseBody) [&] { if constexpr (condition) { return body; } else { return elseBody; } }()

/** @brief Compile-time ternary expression with runtime copy capture */
#define ConstexprTernaryCopy(condition, body, elseBody) [=] { if constexpr (condition) { return body; } else { return elseBody; } }()

namespace kF::Core
{
    /** @brief Theorical cacheline size */
    constexpr std::size_t CacheLineSize = sizeof(std::size_t) * 8;
    constexpr std::size_t CacheLineDoubleSize = CacheLineSize * 2;
    constexpr std::size_t CacheLineHalfSize = CacheLineSize / 2;
    constexpr std::size_t CacheLineQuarterSize = CacheLineSize / 4;
    constexpr std::size_t CacheLineEighthSize = CacheLineSize / 8;

    namespace Utils
    {
        /** @brief Forwards to std::aligned_alloc, but ensure that required alignment is at least 'std::max_align_t' (it cause failure on some systems) */
        template<std::size_t RequiredAlignment>
        [[nodiscard]] inline void *AlignedAlloc(const std::size_t bytes) noexcept
            { return std::aligned_alloc(std::max(alignof(std::max_align_t), RequiredAlignment), bytes); }

        /** @brief Helper to know if a given type is a std::move_iterator */
        template<typename Type>
        struct IsMoveIterator;

        /** @brief Helper that match non-move iterators */
        template<typename Type>
        struct IsMoveIterator
        {
            static constexpr bool Value = false;
        };

        /** @brief Helper that match move iterators */
        template<typename Iterator>
        struct IsMoveIterator<std::move_iterator<Iterator>> : public IsMoveIterator<Iterator>
        {
            static constexpr bool Value = true;
        };

        /** @brief Helper that match reverse iterators */
        template<typename Iterator>
        struct IsMoveIterator<std::reverse_iterator<Iterator>> : public IsMoveIterator<Iterator>
        {};

        static_assert(IsMoveIterator<std::move_iterator<void *>>::Value, "IsMoveIterator not working");
        static_assert(IsMoveIterator<std::reverse_iterator<std::move_iterator<void *>>>::Value, "IsMoveIterator not working");
        static_assert(!IsMoveIterator<std::reverse_iterator<void *>>::Value, "IsMoveIterator not working");
        static_assert(!IsMoveIterator<void *>::Value, "IsMoveIterator not working");


        /** @brief Default type used when a detection fails */
        struct NoneSuch {};

        /** @brief Detector detected invalid expression */
        template<typename Default, typename AlwaysVoid, template<typename...> typename _Op, typename... _Args>
        struct Detector
        {
            using Value = std::false_type;
            using Type = Default;
        };

        /** @brief Detector detected valid expression */
        template<typename Default, template<typename...> typename Op, typename... Args>
        struct Detector<Default, std::void_t<Op<Args...>>, Op, Args...>
        {
            using Value = std::true_type;
            using Type = Op<Args...>;
        };

        /** @brief Boolean that indicate if detector detected a valid expression or not */
        template<template<typename...> class Op, typename... Args>
        constexpr bool IsDetected = Detector<NoneSuch, void, Op, Args...>::Value::value;

        /** @brief Type of the detected expression, if the detection failed returns NoneSuch */
        template<template<typename...> class Op, typename... Args>
        using DetectedType = typename Detector<NoneSuch, void, Op, Args...>::Type;

        /** @brief Type of the detected expression, if the detection failed returns NoneSuch */
        template<typename Default, template<typename...> class Op, typename... Args>
        using DetectedOrType = typename Detector<Default, void, Op, Args...>::Type;

        /** @brief Check if the expression perfectly match a type */
        template<typename Expected, template<typename...> class Op, typename... Args>
        constexpr bool IsDetectedExact = std::is_same_v<Expected, DetectedType<Op, Args...>>;

        /** @brief Check if the expression is convertible to a type */
        template<typename Convertible, template<typename...> class Op, typename... Args>
        constexpr bool IsDetectedConvertible = std::is_convertible_v<Convertible, DetectedType<Op, Args...>>;
    }
}