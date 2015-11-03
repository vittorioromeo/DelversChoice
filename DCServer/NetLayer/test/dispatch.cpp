#include "../include/NetLayer/NetLayer.hpp"
#include "./utils/test_utils.hpp"
#include <string>

struct TestPckt : nl::impl::Pckt<int,  // 0
                      std::string,     // 1
                      float,           // 2
                      std::vector<int> // 3
                      >
{
    using BaseType = nl::impl::Pckt<int, std::string, float, std::vector<int>>;
    using BaseType::BaseType;

    NL_DEFINE_PCKT_PROXY(0, requestID);
    NL_DEFINE_PCKT_PROXY(1, requestUser);
    NL_DEFINE_PCKT_PROXY(2, requestPriority);
    NL_DEFINE_PCKT_PROXY(3, secondaryIDs);
};

int main()
{
    namespace nle = experiment;

    using MySettings = nle::Settings<nl::UInt32>;
    constexpr auto my_binds = nle::pckt_binds<int, float, TestPckt>();
    using MyConfig = decltype(nle::make_config<MySettings>(my_binds));

    using MyDispatchTable = nle::DispatchTable<MyConfig>;

    int outInt = 0;
    float outFloat = 0.f;

    TestPckt outTestPckt;

    MyDispatchTable dt;
    dt.add<int>([&outInt](const auto&, auto&& x)
        {
            outInt = x;
        });

    dt.add<float>([&outFloat](const auto&, auto&& x)
        {
            outFloat = x;
        });

    dt.addDestructured<TestPckt>(
        [&](const auto&, auto&& id, auto&& user, auto&& prio, auto&& secids)
        {
            outTestPckt.requestID() = id;
            outTestPckt.requestUser() = user;
            outTestPckt.requestPriority() = prio;
            outTestPckt.secondaryIDs() = secids;
        });

    auto mkpckt = [](auto x)
    {
        using Type = decltype(x);
        MyConfig::IDType id = MyConfig::template getPcktBindID<Type>();

        nl::PcktBuf b;
        b << id;
        b << x;

        return b;
    };

    for(auto j = 0; j < 100; ++j)
    {
        auto v = ssvu::getRndI(0, 1000);
        auto p = mkpckt((int)v);

        dt.process(nl::PAddress{}, p);
        TEST_ASSERT_OP(v, ==, outInt);
    }

    for(auto j = 0; j < 100; ++j)
    {
        auto v = ssvu::getRndR(0.f, 1000.f);
        auto p = mkpckt((float)v);

        dt.process(nl::PAddress{}, p);
        TEST_ASSERT_OP(v, ==, outFloat);
    }

    for(auto j = 0; j < 100; ++j)
    {
        TestPckt v(nl::init_fields{}, ssvu::getRndR(0.f, 1000.f), "hi",
            ssvu::getRndR(0.f, 1000.f), std::vector<int>{1, 2, 3, 4, 5});
        auto p = mkpckt((TestPckt)v);

        dt.process(nl::PAddress{}, p);
        TEST_ASSERT_NS_OP(v, ==, outTestPckt);
    }

    return 0;
}
