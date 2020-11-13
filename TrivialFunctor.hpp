/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Trivial opaque functor holder
 */

#include <array>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Signature, std::size_t CacheSize = kF::Core::CacheLineEighthSize>
    class TrivialFunctor;

    /** @brief Ensure that a given functor met the trivial requirements of TrivialFunctor */
    template<typename Functor, std::size_t CacheSize>
    concept TrivialFunctorRequirements =
        std::conjunction_v<std::is_trivial<Functor>, std::bool_constant<sizeof(Functor) <= CacheSize>>;
}

/** @brief Very fast opaque functor but only takes trivial types less or equal sized than cacheline eighth */
template<typename Return, typename ...Args, std::size_t CacheSize>
class alignas_quarter_cacheline kF::Core::TrivialFunctor<Return(Args...), CacheSize>
{
public:
    /** @brief Byte cache */
    using Cache = std::array<std::byte, CacheSize>;

    /** @brief Trivial functor signature */
    using OpaqueSignature = Return(*)(Cache &cache, Args...args);

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
    [[nodiscard]] operator bool(void) const noexcept {return _funcPtr; }


    /** @brief Prepare a functor */
    template<typename Functor>
        requires TrivialFunctorRequirements<Functor, CacheSize> && std::invocable<Functor, Args...>
    void prepare(Functor functor) noexcept
    {
        _funcPtr = [](Cache &cache, Args ...args) -> Return {
            return CacheAs<Functor>(cache)(std::forward<Args>(args)...);
        };
        new (&_cache) Functor(std::forward<Functor>(functor));
    }

    /** @brief Prepare a volatile member function */
    template<auto MemberFunction, typename ClassType>
        requires std::invocable<decltype(MemberFunction), ClassType * const, Args...>
    void prepare(ClassType * const instance) noexcept
    {
        _funcPtr = [](Cache &cache, Args ...args) {
            return (CacheAs<ClassType *>(cache)->*MemberFunction)(std::forward<Args>(args)...);
        };
        new (&_cache) ClassType *(instance);
    }

    /** @brief Prepare a const member function */
    template<auto MemberFunction, typename ClassType>
        requires std::invocable<decltype(MemberFunction), const ClassType * const, Args...>
    void prepare(const ClassType * const instance) noexcept
    {
        _funcPtr = [](Cache &cache, Args ...args) {
            return (CacheAs<const ClassType *>(cache)->*MemberFunction)(std::forward<Args>(args)...);
        };
        new (&_cache) const ClassType *(instance);
    }

    /** @brief Prepare a free function */
    template<auto Function>
        requires std::invocable<decltype(Function), Args...>
    void prepare(void) noexcept
    {
        _funcPtr = [](Cache &, Args ...args) -> Return {
            return (*Function)(std::forward<Args>(args)...);
        };
    }

    /** @brief Invoke internal functor */
    Return operator()(Args ...args) const { return _funcPtr(const_cast<Cache &>(_cache), std::forward<Args>(args)...); }

private:
    OpaqueSignature _funcPtr { nullptr };
    Cache _cache;
};