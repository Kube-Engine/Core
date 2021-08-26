/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: SortedVectorDetails
 */

template<typename Base, typename Type, std::integral Range, typename Compare, bool IsSmallOptimized>
template<typename ...Args> requires std::constructible_from<Type, Args...>
inline Type &kF::Core::Internal::SortedVectorDetails<Base, Type, Range, Compare, IsSmallOptimized>::push(Args &&...args)
{
    if (!DetailsBase::data()) [[unlikely]]
        return DetailsBase::push(std::forward<Args>(args)...);
    Type value(std::forward<Args>(args)...);
    auto it = findSortedPlacement(value);
    return *DetailsBase::insert(it, { std::move(value) });
}

template<typename Base, typename Type, std::integral Range, typename Compare, bool IsSmallOptimized>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Range, Compare, IsSmallOptimized>::insertDefault(const Range count)
    noexcept(nothrow_default_constructible(Type) && nothrow_destructible(Type))
{
    if (count) [[likely]]
        DetailsBase::insertDefault(findSortedPlacement(Type{}), count);
}

template<typename Base, typename Type, std::integral Range, typename Compare, bool IsSmallOptimized>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Range, Compare, IsSmallOptimized>::insertCopy(
        const Range count, const Type &value)
{
    if (count) [[likely]]
        DetailsBase::insertCopy(findSortedPlacement(value), count, value);
}

template<typename Base, typename Type, std::integral Range, typename Compare, bool IsSmallOptimized>
template<std::input_iterator InputIterator>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Range, Compare, IsSmallOptimized>::insert(
        InputIterator from, InputIterator to)
{
    if (from != to) [[likely]] {
        DetailsBase::insert(DetailsBase::end(), from, to);
        sort();
    }
}

template<typename Base, typename Type, std::integral Range, typename Compare, bool IsSmallOptimized>
template<std::input_iterator InputIterator, typename Map>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Range, Compare, IsSmallOptimized>::insert(
        InputIterator from, InputIterator to, Map &&map)
{
    if (from != to) [[likely]] {
        DetailsBase::insert(DetailsBase::end(), from, to, std::forward<Map>(map));
        sort();
    }
}

template<typename Base, typename Type, std::integral Range, typename Compare, bool IsSmallOptimized>
template<std::input_iterator InputIterator>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Range, Compare, IsSmallOptimized>::resize(
        InputIterator from, InputIterator to)
{
    if (from != to) [[likely]] {
        DetailsBase::resize(from, to);
        sort();
    }
}

template<typename Base, typename Type, std::integral Range, typename Compare, bool IsSmallOptimized>
template<std::input_iterator InputIterator, typename Map>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Range, Compare, IsSmallOptimized>::resize(
        InputIterator from, InputIterator to, Map &&map)
{
    if (from != to) [[likely]] {
        DetailsBase::resize(from, to, std::forward<Map>(map));
        sort();
    }
}

template<typename Base, typename Type, std::integral Range, typename Compare, bool IsSmallOptimized>
inline void kF::Core::Internal::SortedVectorDetails<Base, Type, Range, Compare, IsSmallOptimized>::sort(void)
{
    std::sort(DetailsBase::beginUnsafe(), DetailsBase::endUnsafe(), Compare{});
}

template<typename Base, typename Type, std::integral Range, typename Compare, bool IsSmallOptimized>
template<typename AssignType>
inline Range kF::Core::Internal::SortedVectorDetails<Base, Type, Range, Compare, IsSmallOptimized>::assign(const Range index, AssignType &&value)
{
    const auto count = DetailsBase::sizeUnsafe();
    auto &elem = DetailsBase::at(index);
    Range finalPos = index;

    elem = std::forward<AssignType>(value);
    if (index > 0 && !Compare{}(DetailsBase::at(index - 1), elem)) {
        finalPos = index - 1;
        for (auto i = index - 2; i > 0; --i) {
            if (!Compare{}(DetailsBase::at(i), elem))
                finalPos = i;
            else
                break;
        }
        std::swap(DetailsBase::at(finalPos), elem);
    } else if (index + 1 < count && !Compare{}(elem, DetailsBase::at(index + 1))) {
        finalPos = index + 1;
        for (auto i = index + 2; i < count; ++i) {
            if (!Compare{}(elem, DetailsBase::at(i))) {
                finalPos = i;
            } else
                break;
        }
        std::swap(DetailsBase::at(finalPos), elem);
    }
    return finalPos;
}