#include "../include/NetLayer/NetLayer.hpp"
#include "./utils/test_utils.hpp"
#include <string>
#include <vector>

int main()
{
    using namespace nl;


    NL_DEFINE_PCKT(Point, (((int), x), ((int), y)));

    static_assert(std::is_base_of<nl::impl::Pckt<int, int>, Point>{}, "");


    NL_DEFINE_PCKT(Line, (((Point), a), ((Point), b)));

    static_assert(std::is_base_of<nl::impl::Pckt<Point, Point>, Line>{}, "");



    NL_DEFINE_PCKT_1(LineCollection, ((std::vector<Line>), lines));

    static_assert(
        std::is_base_of<nl::impl::Pckt<std::vector<Line>>, LineCollection>{},
        "");


    auto make_rnd_point = []
    {
        return Point{
            nl::init_fields{}, ssvu::getRndI(0, 10), ssvu::getRndI(0, 10)};
    };

    auto make_rnd_line = [&]
    {
        return Line{nl::init_fields{}, make_rnd_point(), make_rnd_point()};
    };

    auto make_rnd_line_collection = [&]
    {
        std::vector<Line> lines;
        for(auto i(0); i < 10; ++i) lines.emplace_back(make_rnd_line());
        return make_pckt<LineCollection>(lines);
    };

    constexpr auto test_count(10);

    for(auto i = 0; i < test_count; ++i)
    {
        auto lc = make_rnd_line_collection();

        auto s = nl::make_serialized(lc);
        auto d = nl::make_deserialized<LineCollection>(s);

        TEST_ASSERT_NS_OP(d, ==, lc);
    }

    {
        PAddress a{IpAddr::getLocalAddress(), 10000};

        std::vector<LineCollection> fake_data;
        Tunnel::Fake t;
        t.on_send = [&](auto&& x)
        {
            fake_data.emplace_back(
                nl::make_deserialized<LineCollection>(x.data));

            std::cout << "test\n";
        };

        impl::ManagedSendBuf<Tunnel::Fake> b{t};

        for(auto i = 0; i < test_count; ++i)
        {
            auto lc = make_rnd_line_collection();
            auto res = b.try_enqueue_for(100ms, nl::make_payload(a, lc));

            TEST_ASSERT_OP(res, ==, true);
        }

        bool x;

        while((x = b.send_step()))
        {
        }

        TEST_ASSERT_OP(fake_data.size(), ==, 10);
    }



    return 0;
}
