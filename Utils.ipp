/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Utils
 */

template<std::size_t RequiredAlignment, typename Cast>
inline Cast *kF::Core::Utils::AlignedAlloc(const std::size_t bytes) noexcept
{
    static_assert(RequiredAlignment && ((RequiredAlignment & (RequiredAlignment - 1)) == 0), "Alignment must be a power of 2");

    constexpr auto Alignment = std::max(alignof(std::size_t), RequiredAlignment);

    return reinterpret_cast<Cast *>(std::aligned_alloc(Alignment, (bytes + Alignment - 1) & -Alignment));
}

template<std::integral Unit>
inline constexpr Unit kF::Core::Utils::NextPowerOf2(Unit value)
{
    if (value && !(value & (value - 1)))
        return value;
    auto count = static_cast<Unit>(0);
    while (value != 0) {
        value >>= 1;
        ++count;
    }
    return static_cast<Unit>(1) << count;
}