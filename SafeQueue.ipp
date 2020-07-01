/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SafeQueue
 */

template<typename Type>
kF::SafeQueueProducer<Type> kF::SafeQueue<Type>::acquireProducer(void)
{
    Page *available = nullptr;
    auto readLock = std::shared_lock(_mutex);
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
    readLock.unlock();
    if (state = PageState::Available; available && available->state.compare_exchange_weak(state, PageState::InUse))
        return SafeQueueProducer<Type>(available);
    auto page = std::make_unique<Page>();
    page->state = PageState::InUse;
    auto writeLock = std::lock_guard(_mutex);
    return SafeQueueProducer<Type>(_pages.emplace_back(std::move(page)).get());
}


template<typename Type>
kF::SafeQueueConsumer<Type> kF::SafeQueue<Type>::acquireConsumer(void)
{
    auto readLock = std::shared_lock(_mutex);
    PageState state;

    for (auto &page : _pages) {
        state = page->state.load();
        if (state == PageState::Available && page->state.compare_exchange_weak(state, PageState::InUse))
            return SafeQueueConsumer<Type>(page.get());
    }
    return SafeQueueConsumer<Type>();
}