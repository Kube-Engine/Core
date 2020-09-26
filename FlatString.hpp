/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: FlatString
 */

#include "FlatVector.hpp"

namespace kF::Core
{
    class FlatString;
}

class kF::Core::FlatString : kF::Core::FlatVector<char>
{
public:
    FlatString(void) noexcept = default;
    ~FlatString(void) noexcept { release(); }

    void release(void);

private:
    char *_data { nullptr };
};