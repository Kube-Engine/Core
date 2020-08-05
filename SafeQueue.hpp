/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SafeQueue
 */

#pragma once

#include <vector>
#include <memory>
#include <atomic>
#include <shared_mutex>

namespace kF::Core
{
    template<typename Type>
    class SafeQueue;

    template<typename Type>
    class SafeQueueProducer;

    template<typename Type>
    class SafeQueueConsumer;

    namespace SafeQueueImpl
    {
        /**
         * @brief Enumeration of all possible page states
         *  Empty means that page will be taken in priority for producers
         *  Available means that page contains data for consumers
         *  InUse means that page is used by a producer or a consumer
         */
        enum class SafeQueuePageState {
            Empty,
            Available,
            InUse
        };
    }
}

/**
 * @brief The Safe Queue provide a really fast queue for a versatile multi producers/consumers usage
 *
 * The queue store a set of pages that can be only used in by one thread at a time providing an optimal
 * linear time access without any cpu cache noise (no context switch nor CPU cache invalidation).
 * Each thread must 'acquire' either a 'producer' or a 'consumer' instance to process the data.
 */
template<typename Type>
class kF::Core::SafeQueue
{
public:
    using PageState = SafeQueueImpl::SafeQueuePageState;

    /** @brief A page is just a vector of Type and an atomic state */
    struct Page
    {
        std::vector<Type> data {};
        std::atomic<PageState> state = PageState::Empty;
    };

    /** @brief Default constructor */
    SafeQueue(void) = default;

    /** @brief Emplace a number of page of given prefixed size */
    SafeQueue(const std::size_t pageCount, const std::size_t pageSize);

    /** @brief Move constructor */
    SafeQueue(SafeQueue &&other) noexcept : _pages(std::move(other)) {}

    /** @brief Destructor, not thread safe */
    ~SafeQueue(void) noexcept(std::is_nothrow_destructible_v<Type>) { clear(); }

    /** @brief Move assignment, not thread safe */
    SafeQueue &operator=(SafeQueue &&other) noexcept { _pages.swap(other._pages); }

    /** @brief Acquire a producer, thread safe */
    [[nodiscard]] SafeQueueProducer<Type> acquireProducer(void) noexcept;

    /** @brief Try to acquire a consumer (will return empty instance on failure), thread safe */
    [[nodiscard]] SafeQueueConsumer<Type> acquireConsumer(void) noexcept;

    /** @brief Clear the queue (all pages), not thread safe */
    void clear(void) noexcept(std::is_nothrow_destructible_v<Type>) { _pages.clear(); }

    /** @brief Clear the queue and release all memory */
    void releaseAllMemory(void) noexcept(std::is_nothrow_destructible_v<Type>) { clear(); _pages.shrink_to_fit(); }

private:
    std::vector<std::unique_ptr<Page>> _pages;
    std::shared_mutex _mutex;
};

/**
 * @brief The SafeQueueProducer provide access to a SafeQueue's Page for thread safe insertion
 */
template<typename Type>
class kF::Core::SafeQueueProducer
{
public:
    using Page = typename SafeQueue<Type>::Page;
    using PageState = SafeQueueImpl::SafeQueuePageState;

    /** @brief Default constructor */
    SafeQueueProducer(void) noexcept = default;

    /** @brief Takes ownership of the given page */
    SafeQueueProducer(Page *page) noexcept : _page(page) {}

    /** @brief Move constructor */
    SafeQueueProducer(SafeQueueProducer<Type> &&other) noexcept { std::swap(_page, other.page); }

    /** @brief Destructor, will release the page ownership */
    ~SafeQueueProducer(void) noexcept { if (_page) release(); }

    /** @brief Move assignment */
    SafeQueueProducer &operator=(SafeQueueProducer<Type> &&other) noexcept { std::swap(_page, other.page); return *this; }

    /** @brief Check if the instance has ownership over a page */
    [[nodiscard]] operator bool(void) const noexcept { return _page; }

    /** @brief Retreive internal data */
    [[nodiscard]] auto &data(void) noexcept { return _page->data; }
    [[nodiscard]] const auto &data(void) const noexcept { return _page->data; }

    /** @brief Release the ownership of the page */
    void release(void) noexcept {
        _page->state.store(data().empty() ? PageState::Empty : PageState::Available);
        _page = nullptr;
    }

private:
    Page *_page { nullptr };
};

/**
 * @brief The SafeQueueConsumer provide access to a SafeQueue's Page for thread safe processing
 *
 * When releasing the consumer, the page is destroyed.
 */
template<typename Type>
class kF::Core::SafeQueueConsumer
{
public:
    using Page = typename SafeQueue<Type>::Page;
    using PageState = SafeQueueImpl::SafeQueuePageState;

    /** @brief Default constructor */
    SafeQueueConsumer(void) noexcept = default;

    /** @brief Takes ownership of the given page */
    SafeQueueConsumer(Page *page) noexcept : _page(page) {}

    /** @brief Move constructor */
    SafeQueueConsumer(SafeQueueConsumer<Type> &&other) noexcept { std::swap(_page, other._page); }

    /** @brief Destructor, will release page ownership */
    ~SafeQueueConsumer(void) noexcept(std::is_nothrow_destructible_v<Type>) { if (_page) release(); }

    /** @brief Move assignment */
    SafeQueueConsumer &operator=(SafeQueueConsumer<Type> &&other) noexcept { std::swap(_page, other._page); return *this; }

    /** @brief Check if the instance has ownership over a page */
    [[nodiscard]] operator bool(void) const noexcept { return _page; }

    /** @brief Retreive internal data for processing */
    [[nodiscard]] auto &data(void) noexcept { return _page->data; }
    [[nodiscard]] const auto &data(void) const noexcept { return _page->data; }

    /** @brief Clear and release the ownership of the page */
    void release(void) noexcept(std::is_nothrow_destructible_v<Type>) {
        data().clear();
        _page->state.store(PageState::Empty);
        _page = nullptr;
    }

    /** @brief Release the ownership of the page without clearing it */
    void releaseNoClear(void) noexcept {
        _page->state.store(data().size() == 0 ? PageState::Empty : PageState::Available);
        _page = nullptr;
    }

private:
    Page *_page { nullptr };
};

#include "SafeQueue.ipp"