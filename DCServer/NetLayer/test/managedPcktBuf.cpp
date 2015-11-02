#include "../include/NetLayer/NetLayer.hpp"
#include "./utils/test_utils.hpp"
#include <string>

using namespace std;
using namespace std::literals;
using namespace nl;

void test_send()
{
    PAddress a{IpAddr::getLocalAddress(), 10000};
    Impl::Tunnel::Fake t;
    Impl::ManagedSendBuf<Impl::Tunnel::Fake> b{t};

    int orig_acc = 0;
    int res_acc = 0;

    t.on_send = [&res_acc](auto& p)
    {
        res_acc += nl::make_deserialized<int>(p.data);
    };

    for(auto i = 0; i < 100; ++i)
    {
        orig_acc += i;
        b.try_enqueue_for(10ms, make_payload(a, i));
    }

    while(b.send_step())
    {
    }

    TEST_ASSERT_OP(orig_acc, ==, res_acc);
}

void test_recv() {}

int main()
{
    test_send();
    return 0;
}
