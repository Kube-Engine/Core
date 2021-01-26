/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of Kube's literals
 */

#include <gtest/gtest.h>

#include <Kube/Core/StringLiteral.hpp>

using namespace kF::Literal;

TEST(Literal, FormatStdString)
{
    unsigned char x = 4;
    std::int64_t y = 2;
    std::string res = FormatStdString("Hello", ' ', "World", std::string(", "), std::string_view("life is "), x, y);
    ASSERT_EQ(res, "Hello World, life is 42");
}
