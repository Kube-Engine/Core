/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Trivial opaque functor holder
 */

#include <array>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Signature, std::size_t DesiredCacheSize = CacheLineEighthSize>
    class TrivialFunctor;

    namespace Internal
    {
        /** @brief Ensure that a given functor met the trivial requirements of TrivialFunctor */
        template<typename Functor, std::size_t CacheSize>
        concept TrivialFunctorRequirements =
            std::conjunction_v<std::is_trivial<Functor>, std::bool_constant<sizeof(Functor) <= CacheSize>>;
    };
}

/** @brief Very fast opaque functor but only takes trivial types less or equal sized than cacheline eighth */
template<typename Return, typename ...Args, std::size_t DesiredCacheSize>
class alignas(kF::Core::Utils::NextPowerOf2(kF::Core::CacheLineEighthSize + DesiredCacheSize))
        kF::Core::TrivialFunctor<Return(Args...), DesiredCacheSize>
{
public:
    /** @brief Real cache size */
    static constexpr std::size_t CacheSize = Utils::NextPowerOf2(CacheLineEighthSize + DesiredCacheSize) - CacheLineEighthSize;

    /** @brief Byte cache */
    using Cache = std::array<std::byte, CacheSize>;

    /** @brief Trivial functor signature */
    using OpaqueInvoke = Return(*)(Cache &cache, Args...args);


    /** @brief Cast a cache into a given type */
    template<typename As>
    [[nodiscard]] static inline As &CacheAs(Cache &cache) noexcept
        { return reinterpret_cast<As &>(cache); }


    /** @brief Default constructor */
    TrivialFunctor(void) noexcept = default;

    /** @brief Copy constructor */
    TrivialFunctor(const TrivialFunctor &other) noexcept = default;

    /** @brief Move constructor */
    TrivialFunctor(TrivialFunctor &&other) noexcept = default;

    /** @brief Destructor */
    ~TrivialFunctor(void) noexcept = default;

    /** @brief Copy assignment*/
    TrivialFunctor &operator=(const TrivialFunctor &other) noexcept = default;

    /** @brief Move assignment*/
    TrivialFunctor &operator=(TrivialFunctor &&other) noexcept = default;

    /** @brief Check if the functor is prepared */
    [[nodiscard]] operator bool(void) const noexcept {return _invoke; }


    /** @brief Prepare a functor */
    template<typename ClassFunctor>
        requires Internal::TrivialFunctorRequirements<ClassFunctor, CacheSize> && std::invocable<ClassFunctor, Args...>
    void prepare(ClassFunctor functor) noexcept
    {
        _invoke = [](Cache &cache, Args ...args) -> Return {
            return CacheAs<ClassFunctor>(cache)(std::forward<Args>(args)...);
        };
        new (&_cache) ClassFunctor(std::forward<ClassFunctor>(functor));
    }

    /** @brief Prepare a volatile member function */
    template<auto MemberFunction, typename ClassType>
        requires std::invocable<decltype(MemberFunction), ClassType * const, Args...>
    void prepare(ClassType * const instance) noexcept
    {
        _invoke = [](Cache &cache, Args ...args) {
            return (CacheAs<ClassType *>(cache)->*MemberFunction)(std::forward<Args>(args)...);
        };
        new (&_cache) ClassType *(instance);
    }

    /** @brief Prepare a const member function */
    template<auto MemberFunction, typename ClassType>
        requires std::invocable<decltype(MemberFunction), const ClassType * const, Args...>
    void prepare(const ClassType * const instance) noexcept
    {
        _invoke = [](Cache &cache, Args ...args) {
            return (CacheAs<const ClassType *>(cache)->*MemberFunction)(std::forward<Args>(args)...);
        };
        new (&_cache) const ClassType *(instance);
    }

    /** @brief Prepare a free function */
    template<auto Function>
        requires std::invocable<decltype(Function), Args...>
    void prepare(void) noexcept
    {
        _invoke = [](Cache &, Args ...args) -> Return {
            return (*Function)(std::forward<Args>(args)...);
        };
    }

    /** @brief Invoke internal functor */
    Return operator()(Args ...args) const { return _invoke(const_cast<Cache &>(_cache), std::forward<Args>(args)...); }

private:
    OpaqueInvoke _invoke { nullptr };
    Cache _cache

        /** @brief Cast a cache into a given type */;
};

static_assert_fit(TEMPLATE_TYPE(kF::Core::TrivialFunctor, void(void), kF::Core::CacheLineEighthSize), kF::Core::CacheLineQuarterSize);
static_assert_fit(TEMPLATE_TYPE(kF::Core::TrivialFunctor, void(void), kF::Core::CacheLineQuarterSize), kF::Core::CacheLineHalfSize);
static_assert_fit(TEMPLATE_TYPE(kF::Core::TrivialFunctor, void(void), kF::Core::CacheLineSize), kF::Core::CacheLineDoubleSize);