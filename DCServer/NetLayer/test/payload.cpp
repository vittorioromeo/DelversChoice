#include "../include/NetLayer/NetLayer.hpp"
#include "./utils/test_utils.hpp"
#include <string>

void manualtest()
{
    using namespace std;
    using namespace std::literals;

    using Payload = nl::Payload;

    auto addr = nl::PAddress{"127.0.0.1", 27015};
    Payload p0{addr};

    TEST_ASSERT_OP(addr, ==, p0.address);

    p0.data << 10 << 15.f << 'a';

    int d0;
    float d1;
    char d2;

    p0.data >> d0 >> d1 >> d2;

    TEST_ASSERT_OP(d0, ==, 10);
    TEST_ASSERT_OP(d1, ==, 15.f);
    TEST_ASSERT_OP(d2, ==, 'a');

    nl::debugLo() << p0;
}

void mktest()
{
    using namespace std;
    using namespace std::literals;

    using Payload = nl::Payload;

    auto addr = nl::PAddress{"127.0.0.1", 27015};
    auto p0 = nl::Impl::make_payload(addr, 10, 15.f, 'a');

    TEST_ASSERT_OP(addr, ==, p0.address);

    int d0;
    float d1;
    char d2;

    nl::deserialize(p0.data, d0, d1, d2);

    TEST_ASSERT_OP(d0, ==, 10);
    TEST_ASSERT_OP(d1, ==, 15.f);
    TEST_ASSERT_OP(d2, ==, 'a');

    nl::debugLo() << p0;
}

int main()
{


    manualtest();
    mktest();

    return 0;
}
