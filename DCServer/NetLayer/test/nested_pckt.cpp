#include "../include/NetLayer/NetLayer.hpp"
#include "./utils/test_utils.hpp"
#include <string>

int main()
{
    using namespace nl;
    using namespace nl::Impl;

    NL_DEFINE_PCKT(
        Point, (((int), x), ((int), y)));

    static_assert(std::is_base_of<Pckt<int, int>, Point>{}, "");


    NL_DEFINE_PCKT(
        Line, (((Point), a), ((Point), b)));

    static_assert(std::is_base_of<Pckt<Point, Point>, Line>{}, "");


    {
        Point lr{nl::init_fields{}, 10, 15};
        TEST_ASSERT_OP(lr.x(), ==, 10);
        TEST_ASSERT_OP(lr.y(), ==, 15);

        std::swap(lr.x(), lr.y());
        TEST_ASSERT_OP(lr.x(), ==, 15);
        TEST_ASSERT_OP(lr.y(), ==, 10);

        auto s = nl::make_serialized(lr);
        auto d = nl::make_deserialized<Point>(s);

        TEST_ASSERT_OP(d, ==, lr);
    }

    {
        Point p0{nl::init_fields{}, 5, 10};
        Point p1{nl::init_fields{}, 15, 20};

        TEST_ASSERT_OP(p0.x(), ==, 5);
        TEST_ASSERT_OP(p0.y(), ==, 10);

        TEST_ASSERT_OP(p1.x(), ==, 15);
        TEST_ASSERT_OP(p1.y(), ==, 20);

        Line l0{nl::init_fields{}, p0, p1};
        TEST_ASSERT_OP(l0.a(), ==, p0);
        TEST_ASSERT_OP(l0.b(), ==, p1);
    }



    return 0;
}
