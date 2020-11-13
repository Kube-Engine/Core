/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Trivial functor unit tests
 */

#include <gtest/gtest.h>

#include <Kube/Core/TrivialDispatcher.hpp>

using namespace kF;

struct Foo
{
    int memberFunction(const int x, const int y)
    { return x * y; }

    [[nodiscard]] static int FreeFunction(const int x, const int y)
        { return x * y; }
};

TEST(TrivialFunctor, Basics)
{
    Core::TrivialDispatcher<int(int, int)> dispatcher;
    Foo foo;

    dispatcher.add<&Foo::memberFunction>(&foo);
    dispatcher.add<&Foo::FreeFunction>();
    dispatcher.add([](int x, int y) {
        return x * y;
    });
    ASSERT_EQ(dispatcher.count(), 3);
    auto i = 0u;
    dispatcher.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 3);
    dispatcher.clear();
    i = 0;
    dispatcher.dispatch([&i](int z) { ASSERT_EQ(z, 8); ++i; }, 4, 2);
    ASSERT_EQ(i, 0);
}
