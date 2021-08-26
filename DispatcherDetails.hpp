/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Dispatcher
 */

#pragma once

#include "Vector.hpp"

namespace kF::Core
{
    template<typename Signature, std::size_t CacheSize, template<typename, std::size_t> typename TemplateFunctor>
    class DispatcherDetails;
}

/** @brief Fast event dispatcher */
template<typename Return, typename... Args, std::size_t CacheSize, template<typename, std::size_t> typename TemplateFunctor>
class alignas_quarter_cacheline kF::Core::DispatcherDetails<Return(Args...), CacheSize, TemplateFunctor>
{
public:
    /** @brief Deduced internal functor type */
    using InternalFunctor = TemplateFunctor<Return(Args...), CacheSize>;

    /** @brief Default constructor */
    DispatcherDetails(void) noexcept = default;

    /** @brief Move constructor */
    DispatcherDetails(DispatcherDetails &&dispatcher) noexcept = default;

    /** @brief Destructor */
    ~DispatcherDetails(void) noexcept = default;

    /** @brief Move assignment*/
    DispatcherDetails &operator=(DispatcherDetails &&dispatcher) noexcept = default;


    /** @brief Internal functor count */
    [[nodiscard]] auto count(void) const noexcept { return _functors.size(); }


    /** @brief Add a functor to dispatch list */
    template<typename Functor>
    void add(Functor &&functor) noexcept { _functors.push().prepare(std::forward<Functor>(functor)); }

    /** @brief Add a member function to dispatch list */
    template<auto MemberFunction, typename ClassType>
    void add(ClassType * const instance) noexcept { _functors.push().template prepare<MemberFunction>(instance); }

    /** @brief Add a const member function to dispatch list */
    template<auto MemberFunction, typename ClassType>
    void add(const ClassType * const instance) noexcept { _functors.push().template prepare<MemberFunction>(instance); }

    /** @brief Add a free function to dispatch list */
    template<auto FreeFunction>
    void add(void) noexcept { _functors.push().template prepare<FreeFunction>(); }


    /** @brief Clear dispatch list */
    void clear(void) noexcept_destructible(InternalFunctor) { _functors.clear(); }


    /** @brief Dispatch every internal functors */
    void dispatch(Args ...args)
    {
        for (auto &functor : _functors)
            functor(std::forward<Args>(args)...);
    }

    /** @brief Dispatch every internal functors with a given callback to receive the return value of each functor */
    template<typename Callback>
        requires (!std::is_same_v<Return, void> && std::invocable<Callback, Return>)
    void dispatch(Callback &&callback, Args ...args)
    {
        for (auto &functor : _functors)
            callback(functor(std::forward<Args>(args)...));
    }

private:
    TinyVector<InternalFunctor> _functors {};
};