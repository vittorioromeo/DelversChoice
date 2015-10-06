#include "../include/NetLayer/NetLayer.hpp"
#include "./utils/test_utils.hpp"
#include <string>


int main()
{
    using namespace std;
    using namespace std::literals;

#define PRIMITIVE_TEST(type, val)     \
    {                                 \
        nl::PcktBuf pb;               \
        pb << static_cast<type>(val); \
                                      \
        type r;                       \
        pb >> r;                      \
                                      \
        TEST_ASSERT_OP(r, ==, val);   \
    }

    PRIMITIVE_TEST(int, 0)
    PRIMITIVE_TEST(float, 1.5f)
    PRIMITIVE_TEST(double, 2.5)
    PRIMITIVE_TEST(std::string, "abcd"s)

    // ---

    {
        nl::PcktBuf pb;
        pb << std::vector<int>{0, 1, 2, 3, 4, 5};

        std::vector<int> r;
        pb >> r;

        TEST_ASSERT_OP(r[0], ==, 0);
        TEST_ASSERT_OP(r[1], ==, 1);
        TEST_ASSERT_OP(r[2], ==, 2);
        TEST_ASSERT_OP(r[3], ==, 3);
        TEST_ASSERT_OP(r[4], ==, 4);
        TEST_ASSERT_OP(r[5], ==, 5);
    }

    {
        nl::PcktBuf pb;
        pb << std::vector<std::vector<float>>{
            std::vector<float>{1.1f, 1.2f}, std::vector<float>{1.3f}};

        std::vector<std::vector<float>> r;
        pb >> r;

        TEST_ASSERT_OP(r[0][0], ==, 1.1f);
        TEST_ASSERT_OP(r[0][1], ==, 1.2f);
        TEST_ASSERT_OP(r[1][0], ==, 1.3f);
    }



    {
        auto o = make_tuple(1, 2.f, "ciao"s);

        nl::PcktBuf pb;
        pb << o;

        decltype(o) r;
        pb >> r;

        TEST_ASSERT_NS_OP(r, ==, o);
    }

    {
        auto o = make_tuple(
            1, 2.f, make_tuple(1, 2, std::vector<int>{1, 2}), "ciao"s);

        nl::PcktBuf pb;
        pb << o;

        decltype(o) r;
        pb >> r;

        TEST_ASSERT_NS_OP(r, ==, o);
    }

    return 0;
}
