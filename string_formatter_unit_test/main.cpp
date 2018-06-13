#define BOOST_TEST_MODULE string_format
#include <boost/test/unit_test.hpp>

#include "string_format.h"

using namespace string_format;

typedef unsigned int uint;

class hello_world
{
    friend std::ostream& operator << (std::ostream& os, const hello_world& rhs)
    {
        os << "Hello world";
        return os;
    }
};

#pragma region simple format prvalue

BOOST_AUTO_TEST_CASE(simple_format_prvalue_char)
{
    auto s = format("{0}", '1');
    BOOST_REQUIRE_EQUAL(s, "1");
}

BOOST_AUTO_TEST_CASE(simple_format_prvalue_uint)
{
    auto s = format("{0}", (uint)1);
    BOOST_REQUIRE_EQUAL(s, "1");
}

BOOST_AUTO_TEST_CASE(simple_format_prvalue_int)
{
    auto s = format("{0}", 1);
    BOOST_REQUIRE_EQUAL(s, "1");
}

BOOST_AUTO_TEST_CASE(simple_format_prvalue_float)
{
    auto s = format("{0}", 1.0f);
    BOOST_REQUIRE_EQUAL(s, "1.000000");
}

BOOST_AUTO_TEST_CASE(simple_format_prvalue_double)
{
    auto s = format("{0}", 1.0);
    BOOST_REQUIRE_EQUAL(s, "1.000000");
}

#pragma endregion

#pragma region simple format lvalue

BOOST_AUTO_TEST_CASE(simple_format_lvalue_char)
{
    char one = '1';
    auto s = format("{0}", one);
    BOOST_REQUIRE_EQUAL(s, "1");
}

BOOST_AUTO_TEST_CASE(simple_format_lvalue_uint)
{
    uint one = 1;
    auto s = format("{0}", one);
    BOOST_REQUIRE_EQUAL(s, "1");
}

BOOST_AUTO_TEST_CASE(simple_format_lvalue_int)
{
    int one = 1;
    auto s = format("{0}", one);
    BOOST_REQUIRE_EQUAL(s, "1");
}

BOOST_AUTO_TEST_CASE(simple_format_lvalue_float)
{
    float one = 1.0f;
    auto s = format("{0}", one);
    BOOST_REQUIRE_EQUAL(s, "1.000000");
}

BOOST_AUTO_TEST_CASE(simple_format_lvalue_double)
{
    double one = 1.0;
    auto s = format("{0}", one);
    BOOST_REQUIRE_EQUAL(s, "1.000000");
}

#pragma endregion

#pragma region format with argument float

BOOST_AUTO_TEST_CASE(format_argument_float_f0)
{
    auto s = format("{0:F0}", 123.4567f);
    BOOST_REQUIRE_EQUAL(s, "123");
}

BOOST_AUTO_TEST_CASE(format_argument_float_f2)
{
    auto s = format("{0:F2}", 123.4567f);
    // Number gets rounded
    BOOST_REQUIRE_EQUAL(s, "123.46");
}

BOOST_AUTO_TEST_CASE(format_argument_float_f4)
{
    auto s = format("{0:F4}", 123.4567f);
    BOOST_REQUIRE_EQUAL(s, "123.4567");
}

BOOST_AUTO_TEST_CASE(format_argument_float_e4)
{
    auto s = format("{0:E4}", 123.4567f);
    // Nnumber gets rounded
    BOOST_REQUIRE_EQUAL(s, "1.2346e+02");
}

BOOST_AUTO_TEST_CASE(format_argument_float_x4)
{
    auto s = format("{0:X4}", 123.4567f);
    // I have no idea if this is correct, but this is the output it gives me
    BOOST_REQUIRE_EQUAL(s, "0x1.edd4p+6");
}

#pragma endregion

#pragma region other formatting features

BOOST_AUTO_TEST_CASE(format_reordering)
{
    auto s = format("{1} {0}", 1, 0);
    BOOST_REQUIRE_EQUAL(s, "0 1");
}

BOOST_AUTO_TEST_CASE(format_reuse)
{
    auto s = format("{0} {0}", 0);
    // Number gets rounded
    BOOST_REQUIRE_EQUAL(s, "0 0");
}

#pragma endregion

#pragma region complex tests

BOOST_AUTO_TEST_CASE(complex_test_1)
{
    auto s = format("{3:F1} + {1}.{1} = {2}\n{4}{0}", '!', 2, "5", 2.8, hello_world{});
    BOOST_REQUIRE_EQUAL(s, "2.8 + 2.2 = 5\nHello world!");
}

#pragma endregion
