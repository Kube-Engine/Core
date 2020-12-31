/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Trivial opaque functor holder
 */

#include <array>

#include "Utils.hpp"

namespace kF::Core
{
    template<typename Signature, std::size_t CacheSize = CacheLineEighthSize>
    class TrivialFunctor;

    namespace Internal
    {
        /** @brief Ensure that a given functor met the trivial requirements of TrivialFunctor */
        template<typename Functor, std::size_t CacheSize>
        concept TrivialFunctorRequirements =
            std::conjunction_v<std::is_trivially_copy_constructible<Functor>, std::bool_constant<sizeof(Functor) <= CacheSize>>;

        /** @brief Ensure that a given functor / function is callable */
        template<typename Functor, typename Return, typename ...Args>
        concept TrivialFunctorInvocable = std::is_invocable_r_v<Return, Functor, Args...>;

        /** @brief Ensure that a given member function is callable */
        template<auto Member, typename ClassType, typename Return, typename ...Args>
        concept TrivialFunctorMemberInvocable = std::is_invocable_r_v<Return, decltype(Member), ClassType &, Args...>;
    };
}

/** @brief Very fast opaque functor but only takes trivial types less or equal sized than cacheline eighth */
template<typename Return, typename ...Args, std::size_t CacheSize>
class alignas_eighth_cacheline kF::Core::TrivialFunctor<Return(Args...), CacheSize>
{
public:
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

    /** @brief Prepare constructor, limited to runtime functors due to template constructor restrictions */
    template<typename ClassFunctor>
        requires (!std::is_same_v<TrivialFunctor, std::remove_cvref_t<ClassFunctor>>) && Internal::TrivialFunctorInvocable<ClassFunctor, Return, Args...>
    TrivialFunctor(ClassFunctor &&functor) noexcept { prepare(std::forward<ClassFunctor>(functor)); }


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
        requires Internal::TrivialFunctorRequirements<ClassFunctor, CacheSize>
            && Internal::TrivialFunctorInvocable<ClassFunctor, Return, Args...>
    void prepare(ClassFunctor &&functor) noexcept
    {
        _invoke = [](Cache &cache, Args ...args) -> Return {
            return CacheAs<ClassFunctor>(cache)(std::forward<Args>(args)...);
        };
        new (&_cache) ClassFunctor(std::forward<ClassFunctor>(functor));
    }

    /** @brief Prepare a volatile member function */
    template<auto MemberFunction, typename ClassType>
        requires Internal::TrivialFunctorMemberInvocable<MemberFunction, ClassType, Return, Args...>
    void prepare(ClassType * const instance) noexcept
    {
        _invoke = [](Cache &cache, Args ...args) {
            return (CacheAs<ClassType *>(cache)->*MemberFunction)(std::forward<Args>(args)...);
        };
        new (&_cache) ClassType *(instance);
    }

    /** @brief Prepare a const member function */
    template<auto MemberFunction, typename ClassType>
        requires Internal::TrivialFunctorMemberInvocable<MemberFunction, const ClassType, Return, Args...>
    void prepare(const ClassType * const instance) noexcept
    {
        _invoke = [](Cache &cache, Args ...args) {
            return (CacheAs<const ClassType *>(cache)->*MemberFunction)(std::forward<Args>(args)...);
        };
        new (&_cache) const ClassType *(instance);
    }

    /** @brief Prepare a free function */
    template<auto Function>
        requires Internal::TrivialFunctorInvocable<decltype(Function), Return, Args...>
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
    Cache _cache;
};