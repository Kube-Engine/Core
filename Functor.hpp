/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Functor
 */

#pragma once

#include <array>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Signature, std::size_t DesiredCacheSize = CacheLineQuarterSize>
    class Functor;

    namespace Internal
    {
        /** @brief Ensure that a given functor met the trivial requirements of Functor */
        template<typename Functor, std::size_t CacheSize>
        concept FunctorCacheRequirements =
            std::conjunction_v<std::is_trivial<Functor>, std::bool_constant<sizeof(Functor) <= CacheSize>>;
    }
}

/** @brief Very fast opaque functor but only takes trivial types less or equal sized than cacheline eighth */
template<typename Return, typename ...Args, std::size_t DesiredCacheSize>
class alignas(kF::Core::Utils::NextPowerOf2(kF::Core::CacheLineQuarterSize + DesiredCacheSize))
        kF::Core::Functor<Return(Args...), DesiredCacheSize>
{
public:
    /** @brief Real cache size */
    static constexpr std::size_t CacheSize = std::max(
        Utils::NextPowerOf2(CacheLineQuarterSize + DesiredCacheSize) - CacheLineQuarterSize,
        CacheLineEighthSize
    );

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

    /** @brief Destructor */
    ~Functor(void) noexcept { destroy(); }

    /** @brief Move assignment*/
    Functor &operator=(Functor &&other) noexcept
    {
        destroy();
        if (other._destruct) {
            _invoke = other._invoke;
            _destruct = other._destruct;
        } else {
            _invoke = other._invoke;
            _destruct = nullptr;
            _cache = other._cache;
        }
        other._invoke = nullptr;
        other._destruct = nullptr;
        return *this;
    }

    /** @brief Check if the functor is prepared */
    [[nodiscard]] operator bool(void) const noexcept { return _invoke; }


    /** @brief Prepare a trivial functor */
    template<typename ClassFunctor>
        requires Internal::FunctorCacheRequirements<ClassFunctor, CacheSize> && std::invocable<ClassFunctor, Args...>
    void prepare(ClassFunctor functor) noexcept
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
    void prepare(ClassFunctor functor) noexcept
    {
        using ClassFunctorPtr = std::unique_ptr<ClassFunctor>;

        destroy();
        _invoke = [](Cache &cache, Args ...args) -> Return {
            return (*CacheAs<ClassFunctorPtr>(cache))(std::forward<Args>(args)...);
        };
        _destruct = [](Cache &cache) {
            CacheAs<ClassFunctorPtr>(cache).~ClassFunctorPtr();
        };
        new (&_cache) ClassFunctorPtr(
            std::make_unique<ClassFunctor>(std::forward<ClassFunctor>(functor))
        );
    }

    /** @brief Prepare a volatile member function */
    template<auto MemberFunction, typename ClassType>
        requires std::invocable<decltype(MemberFunction), ClassType * const, Args...>
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
        requires std::invocable<decltype(MemberFunction), const ClassType * const, Args...>
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
        requires std::invocable<decltype(Function), Args...>
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

static_assert_fit(TEMPLATE_TYPE(kF::Core::Functor, void(void), kF::Core::CacheLineEighthSize), kF::Core::CacheLineHalfSize);
static_assert_fit(TEMPLATE_TYPE(kF::Core::Functor, void(void), kF::Core::CacheLineQuarterSize), kF::Core::CacheLineHalfSize);
static_assert_fit(TEMPLATE_TYPE(kF::Core::Functor, void(void), kF::Core::CacheLineHalfSize), kF::Core::CacheLineSize);
static_assert_fit(TEMPLATE_TYPE(kF::Core::Functor, void(void), kF::Core::CacheLineSize), kF::Core::CacheLineDoubleSize);