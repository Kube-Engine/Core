/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: String utils
 */

#pragma once

#include <ostream>

#include "StringDetails.hpp"

template<typename Base, typename Type, typename Range>
inline std::ostream &operator<<(std::ostream &stream, const kF::Core::Internal::StringDetails<Base, Type, Range> &str)
{
    return stream << str.toStdView();
}