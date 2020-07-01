/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Version
 */

#pragma once

#include <cinttypes>

namespace kF
{
    struct Version;
}

/**
 * @brief Structure representation of a version
 */
struct kF::Version
{
    /** @brief Default constructor */
    constexpr Version(std::uint8_t major_ = 0, std::uint8_t minor_ = 0, std::uint8_t patch_ = 0)
        : major(major_), minor(minor_), patch(patch_) {}

    std::uint8_t major = 0;
    std::uint8_t minor = 0;
    std::uint8_t patch = 0;
};