/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Functor
 */

#pragma once

#include <array>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Signature, std::size_t CacheSize = CacheLineQuarterSize>
    class Functor;

    namespace Internal
    {
        /** @brief Ensure that a given functor met the trivial requirements of Functor */
        template<typename Functor, std::size_t CacheSize>
        concept FunctorCacheRequirements = std::is_trivial_v<Functor> && sizeof(Functor) <= CacheSize;

        /** @brief Ensure that a given functor DOES NOT met the trivial requirements of Functor */
        template<typename Functor, std::size_t CacheSize>
        concept FunctorNoCacheRequirements = (std::is_trivial_v<Functor> ? sizeof(Functor) > CacheSize : true);

        /** @brief Ensure that a given functor / function is callable */
        template<typename Functor, typename Return, typename ...Args>
        concept FunctorInvocable = std::is_invocable_r_v<Return, Functor, Args...>;

        /** @brief Ensure that a given member function is callable */
        template<auto Member, typename ClassType, typename Return, typename ...Args>
        concept FunctorMemberInvocable = std::is_invocable_r_v<Return, decltype(Member), ClassType &, Args...>;
    }
}

/** @brief Very fast opaque functor but only takes trivial types less or equal sized than cacheline eighth */
template<typename Return, typename ...Args, std::size_t CacheSize>
class kF::Core::Functor<Return(Args...), CacheSize>
{
public:
    /** @brief Byte cache */
    using Cache = std::array<std::byte, CacheSize>;

    /** @brief Functor signature */
    using OpaqueInvoke = Return(*)(Cache &cache, Args...args);
    using OpaqueDestructor = void(*)(Cache &data);


    /** @brief Cast a cache into a given type */
    template<typename As>
    [[nodiscard]] static inline As &CacheAs(Cache &cache) noexcept
        { return reinterpret_cast<As &>(cache); }


    /** @brief Default constructor */
    Functor(void) noexcept = default;

    /** @brief Move constructor */
    Functor(Functor &&other) noexcept
    {
        _invoke = other._invoke;
        _destruct = other._destruct;
        _cache = other._cache;
        other._invoke = nullptr;
        other._destruct = nullptr;
    }

    /** @brief Prepare constructor, limited to runtime functors due to template constructor restrictions */
    template<typename ClassFunctor>
        requires (!std::is_same_v<Functor, std::remove_cvref_t<ClassFunctor>>) && Internal::FunctorInvocable<ClassFunctor, Return, Args...>
    Functor(ClassFunctor &&functor) noexcept_forward_constructible(decltype(functor))
        { prepare(std::forward<ClassFunctor>(functor)); }

    /** @brief Destructor */
    ~Functor(void) { destroy(); }

    /** @brief Move assignment*/
    Functor &operator=(Functor &&other) noexcept
    {
        destroy();
        _invoke = other._invoke;
        _destruct = other._destruct;
        _cache = other._cache;
        other._invoke = nullptr;
        other._destruct = nullptr;
        return *this;
    }

    /** @brief Prepare constructor, limited to runtime functors due to template constructor restrictions */
    template<typename ClassFunctor>
        requires (!std::is_same_v<Functor, std::remove_cvref_t<ClassFunctor>>) && Internal::FunctorInvocable<ClassFunctor, Return, Args...>
    Functor &operator=(ClassFunctor &&functor) noexcept_forward_constructible(decltype(functor))
        { prepare(std::forward<ClassFunctor>(functor)); return *this; }


    /** @brief Check if the functor is prepared */
    [[nodiscard]] operator bool(void) const noexcept { return _invoke; }


    /** @brief Prepare a trivial functor */
    template<typename ClassFunctor>
        requires Internal::FunctorCacheRequirements<ClassFunctor, CacheSize> && Internal::FunctorInvocable<ClassFunctor, Return, Args...>
    void prepare(ClassFunctor &&functor) noexcept
    {
        destroy();
        _invoke = [](Cache &cache, Args ...args) -> Return {
            return CacheAs<ClassFunctor>(cache)(std::forward<Args>(args)...);
        };
        _destruct = nullptr;
        new (&_cache) ClassFunctor(std::forward<ClassFunctor>(functor));
    }

    /** @brief Prepare a non-trivial functor */
    template<typename ClassFunctor>
        requires Internal::FunctorNoCacheRequirements<ClassFunctor, CacheSize> && Internal::FunctorInvocable<ClassFunctor, Return, Args...>
    void prepare(ClassFunctor &&functor) noexcept_forward_constructible(decltype(functor))
    {
        using FlatClassFunctor = std::remove_cvref_t<ClassFunctor>;
        using ClassFunctorPtr = FlatClassFunctor *;

        destroy();
        _invoke = [](Cache &cache, Args ...args) -> Return {
            if constexpr (std::is_same_v<Return, void>)
                (*CacheAs<ClassFunctorPtr>(cache))(std::forward<Args>(args)...);
            else
                return (*CacheAs<ClassFunctorPtr>(cache))(std::forward<Args>(args)...);
        };
        _destruct = [](Cache &cache) {
            delete CacheAs<ClassFunctorPtr>(cache);
        };
        CacheAs<ClassFunctorPtr>(_cache) = new FlatClassFunctor(std::forward<ClassFunctor>(functor));
    }

    /** @brief Prepare a volatile member function */
    template<auto MemberFunction, typename ClassType>
        requires Internal::FunctorMemberInvocable<MemberFunction, ClassType, Return, Args...>
    void prepare(ClassType * const instance) noexcept
    {
        destroy();
        _invoke = [](Cache &cache, Args ...args) {
            return (CacheAs<ClassType *>(cache)->*MemberFunction)(std::forward<Args>(args)...);
        };
        _destruct = nullptr;
        new (&_cache) ClassType *(instance);
    }

    /** @brief Prepare a const member function */
    template<auto MemberFunction, typename ClassType>
        requires Internal::FunctorMemberInvocable<MemberFunction, const ClassType, Return, Args...>
    void prepare(const ClassType * const instance) noexcept
    {
        destroy();
        _invoke = [](Cache &cache, Args ...args) {
            return (CacheAs<const ClassType *>(cache)->*MemberFunction)(std::forward<Args>(args)...);
        };
        _destruct = nullptr;
        new (&_cache) const ClassType *(instance);
    }

    /** @brief Prepare a free function */
    template<auto Function>
        requires Internal::FunctorInvocable<decltype(Function), Return, Args...>
    void prepare(void) noexcept
    {
        destroy();
        _invoke = [](Cache &, Args ...args) -> Return {
            return (*Function)(std::forward<Args>(args)...);
        };
        _destruct = nullptr;
    }

    /** @brief Invoke internal functor */
    Return operator()(Args ...args) const { return _invoke(const_cast<Cache &>(_cache), std::forward<Args>(args)...); }

private:
    OpaqueInvoke _invoke { nullptr };
    OpaqueDestructor _destruct { nullptr };
    Cache _cache;

    /** @brief Destroy the holded instance */
    void destroy(void)
    {
        if (_destruct)
            _destruct(_cache);
    }
};