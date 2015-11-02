#include "../include/NetLayer/NetLayer.hpp"
#include "./utils/test_utils.hpp"
#include <string>

using namespace std;
using namespace std::literals;
using namespace nl;
using namespace nl::Impl;

int main()
{
    bool success{false};
    bool success_send{false};

    PAddress some_address(nl::IpAddr::getLocalAddress(), 27015);
    PAddress some_sender_addr(nl::IpAddr::getLocalAddress(), 17015);

    ManagedHostImpl<Tunnel::Fake>* cp;

    Tunnel::Fake t;
    t.on_recv = [&](auto& p)
    {
        if(!cp->busy()) return false;

        p = make_payload(some_address, 1234);
        return true;
    };

    t.on_send = [&](auto& p)
    {
        if(!cp->busy()) return false;

        TEST_ASSERT_OP(p.address, ==, some_sender_addr);

        int ds_data = nl::make_deserialized<int>(p.data);
        TEST_ASSERT_OP(ds_data, ==, 4321);

        success_send = true;
        return true;
    };

    ManagedHostImpl<Tunnel::Fake> client{27016, t};
    cp = &client;

    auto fnProcess([&](auto& data, const PAddress& a)
        {
            TEST_ASSERT_OP(a, ==, some_address);
            int ds_data = nl::make_deserialized<int>(data);
            TEST_ASSERT_OP(ds_data, ==, 1234);

            success = true;
        });

    client.emplace_busy_loop([&client, &fnProcess]
        {
            client.try_process(fnProcess);
        });

    int iters = 10;

    while(client.busy())
    {
        auto p(make_payload(some_sender_addr, 4321));
        client.send(p);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        --iters;

        if(iters <= 0) client.stop();
    }

    TEST_ASSERT_OP(success, ==, true);
    TEST_ASSERT_OP(success_send, ==, true);
}
