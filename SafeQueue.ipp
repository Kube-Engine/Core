/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SafeQueue
 */

template<typename Type>
kF::Core::SafeQueueProducer<Type> kF::Core::SafeQueue<Type>::acquireProducer(void) noexcept
{
    Page *available = nullptr;
    std::shared_lock readLock(_mutex);
    PageState state;

    for (auto &page : _pages) {
        state = page->state.load();
        switch (state) {
        case PageState::Empty:
            if (!page->state.compare_exchange_weak(state, PageState::InUse))
                break;
            return SafeQueueProducer<Type>(page.get());
        case PageState::Available:
            available = page.get();
            break;
        case PageState::InUse:
            break;
        }
    }
    if (state = PageState::Available; available && available->state.compare_exchange_weak(state, PageState::InUse))
        return SafeQueueProducer<Type>(available);
    readLock.unlock();
    auto page = std::make_unique<Page>();
    page->state = PageState::InUse;
    std::lock_guard writeLock(_mutex);
    return SafeQueueProducer<Type>(_pages.emplace_back(std::move(page)).get());
}

template<typename Type>
kF::Core::SafeQueueConsumer<Type> kF::Core::SafeQueue<Type>::acquireConsumer(void) noexcept
{
    PageState state;

    for (std::shared_lock readLock(_mutex); auto &page : _pages) {
        state = page->state.load();
        if (state == PageState::Available && page->state.compare_exchange_weak(state, PageState::InUse))
            return SafeQueueConsumer<Type>(page.get());
    }
    return SafeQueueConsumer<Type>();
}