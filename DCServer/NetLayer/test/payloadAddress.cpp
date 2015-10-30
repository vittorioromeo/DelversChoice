#include "../include/NetLayer/NetLayer.hpp"
#include "./utils/test_utils.hpp"
#include <string>


int main()
{
    using namespace std;
    using namespace std::literals;

    nl::Impl::PayloadAddress pa{"127.0.0.1", 27015};

    TEST_ASSERT_OP(pa.ip, ==, "127.0.0.1");
    TEST_ASSERT_OP(pa.port, ==, 27015);

    nl::debugLo() << pa;

    return 0;
}
