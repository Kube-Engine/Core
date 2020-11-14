/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Utils
 */

template<std::size_t RequiredAlignment>
[[nodiscard]] inline void *kF::Core::Utils::AlignedAlloc(const std::size_t bytes) noexcept
{
    static_assert(RequiredAlignment && ((RequiredAlignment & (RequiredAlignment - 1)) == 0), "Alignment must be a power of 2");

    constexpr auto Alignment = std::max(alignof(std::size_t), RequiredAlignment);

    // Macos standard library is uncomplete !
    if constexpr (Core::Utils::IsDetected<HasAlignedAlloc, std::size_t>)
        return std::aligned_alloc(Alignment, (bytes + Alignment - 1) & -Alignment);
    else
        return std::malloc(bytes);
}
