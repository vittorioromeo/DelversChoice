#include "../include/NetLayer/NetLayer.hpp"
#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"

// TODO:
// * decide abstraction layers
// * decide packet definition and settings definitionsyntax
// * implement an additional layer that abstracts the Architecture module over
// PacketTypes
// * test stuff.
// * acks/reliability stuff.

namespace tests
{
namespace nle = experiment;

using MySettings = nle::Settings<std::size_t>;

using MyPcktBinds = nle::PcktBinds<nle::PcktBind<int>, nle::PcktBind<float>>;

using MyConfig = nle::Config<MySettings, MyPcktBinds>;
}
 
template <typename T>
T getInput(const std::string& mTitle)
{
    T input;
    std::cin >> input;

    ssvu::lo(mTitle) << input << "\n";
    return input;
}


void choiceServer()
{
    /*ssvu::lo() << "Insert port:\n";
    auto port(getInput<nl::Port>("Port"));
    nl::ManagedSocket server{port};*/

    auto processedCount(0u);

    auto fnProcess(
    [&processedCount](auto& server, auto& data, const auto& sender)
    {
        ++processedCount;

        ssvu::lo() << "Received some data from " << sender << "!\n";

        std::string str;
        data >> str;

        ssvu::lo() << "Data: " << str << "\n";

        server.send(sender, "I got your message!"s);
    });

    nl::ManagedHost<decltype(fnProcess)> server{27015, fnProcess};
    // auto server(nl::mkManagedHost(27015, fnProcess));
    // auto server(nl::makeManagedHost(27015, fnProcess));

    int cycles{20};

    while(server.isBusy()) {

        if(getInput<int>("Exit? (1)")) break;

        // NL_DEBUGLO() << "bsy";
        if(cycles-- <= 0) {
            // server.stop();
        }

        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // ssvu::lo() << "...\n";

        // if(processedCount > 0)
        //    ssvu::lo() << "Processed packets: " << processedCount << "\n\n";
    }

    NL_DEBUGLO() << "serverend\n";
}

void choiceClient()
{
    /*ssvu::lo() << "Insert port:\n";
    auto port(getInput<nl::Port>("Port"));
    nl::ManagedSocket client{port};*/

    auto fnProcess([](auto&, auto& data, const auto&)
    {
        std::string str;
        data >> str;

        ssvu::lo() << "Reply: " << str << "\n";
    });

    nl::ManagedHost<decltype(fnProcess)> client{27016, fnProcess};

    while(client.isBusy()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));



        nl::Impl::PayloadTarget myself(nl::IpAddr::getLocalAddress(), 27015);
        client.send(myself, "hello!"s);



        // ssvu::lo() << "...\n";
    }
}

namespace example
{
NL_DEFINE_PCKT(
RegistrationRequest, (((std::string), user), ((std::string), pass)));

NL_DEFINE_PCKT_1(RegistrationResponse, ((bool), valid));

NL_DEFINE_PCKT(LoginRequest, (((std::string), user), ((std::string), pass)));

NL_DEFINE_PCKT_1(LoginResponse, ((bool), valid));

namespace nle = experiment;
using MySettings = nle::Settings<std::size_t>;

using MyPcktBinds =
nle::PcktBinds<nle::PcktBind<RegistrationRequest>, nle::PcktBind<LoginRequest>,
nle::PcktBind<RegistrationResponse>, nle::PcktBind<LoginResponse>>;

using MyConfig = nle::Config<MySettings, MyPcktBinds>;

static_assert(MyConfig::getPcktBindID<RegistrationRequest>() == 0, "");
static_assert(MyConfig::getPcktBindID<LoginRequest>() == 1, "");
static_assert(MyConfig::getPcktBindID<RegistrationResponse>() == 2, "");
static_assert(MyConfig::getPcktBindID<LoginResponse>() == 3, "");

}

int main()
{
    ssvu::lo("Choose") << "\n"
                       << "0. Server\n"
                       << "1. Client\n"
                       << "_. Exit\n";

    auto choice(getInput<int>("Choice"));

    if(choice == 0) {
        choiceServer();
        NL_DEBUGLO() << "end choiceserver";
    }
    else if(choice == 1)
    {
        choiceClient();
    }
    else
    {
        std::terminate();
    }

    NL_DEBUGLO() << "return0 ";
    return 0;
}
