#include "../include/NetLayer/NetLayer.hpp"
#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"

// TODO:
// * decide abstraction layers
// * decide packet definition and settings definitionsyntax
// * implement an additional layer that abstracts the Architecture module over
// PacketTypes
// * test stuff.
// * acks/reliability stuff.

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

    nl::ManagedHost server{27015};

    auto fnProcess([&processedCount, &server](auto& data, const auto& sender)
        {
            ++processedCount;

            ssvu::lo() << "Received some data from " << sender << "!\n";

            std::string str;
            data >> str;

            ssvu::lo() << "Data: " << str << "\n";

            server.send(sender, "I got your message!"s);
        });

    server.emplaceBusyFut([&server, &fnProcess]
        {
            server.try_process(fnProcess);
        });


    // auto server(nl::mkManagedHost(27015, fnProcess));
    // auto server(nl::makeManagedHost(27015, fnProcess));

    int cycles{20};

    while(server.isBusy())
    {

        if(getInput<int>("Exit? (1)")) break;

        // NL_DEBUGLO() << "bsy";
        if(cycles-- <= 0)
        {
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

    nl::ManagedHost client{27016};

    auto fnProcess([](auto& data, const auto&)
        {
            std::string str;
            data >> str;

            ssvu::lo() << "Reply: " << str << "\n";
        });

    client.emplaceBusyFut([&client, &fnProcess]
        {
            client.try_process(fnProcess);
        });


    while(client.isBusy())
    {

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

    NL_DEFINE_PCKT(
        LoginRequest, (((std::string), user), ((std::string), pass)));

    NL_DEFINE_PCKT_1(LoginResponse, ((bool), valid));

    namespace nle = experiment;

    using MySettings = nle::Settings<nl::UInt32>;

    using MyPcktBinds = nle::PcktBinds<nle::PcktBind<RegistrationRequest>,
        nle::PcktBind<LoginRequest>, nle::PcktBind<RegistrationResponse>,
        nle::PcktBind<LoginResponse>>;

    using MyConfig = nle::Config<MySettings, MyPcktBinds>;

    using MyContextHost = nle::ContextHost<MyConfig>;

    static_assert(MyConfig::getPcktBindID<RegistrationRequest>() == 0, "");
    static_assert(MyConfig::getPcktBindID<LoginRequest>() == 1, "");
    static_assert(MyConfig::getPcktBindID<RegistrationResponse>() == 2, "");
    static_assert(MyConfig::getPcktBindID<LoginResponse>() == 3, "");


    void startServer()
    {
        MyContextHost h{27015};

        h.on_in<RegistrationRequest>(
            [](const auto&, const auto& user, const auto& pass)
            {
                ssvu::lo() << "registration request from " << user
                           << ", pass: " << pass << "\n";
            });

        h.on_in<LoginRequest>(
            [](const auto&, const auto& user, const auto& pass)
            {
                ssvu::lo() << "login request from " << user
                           << ", pass: " << pass << "\n";
            });

        while(h.busy())
        {
            std::this_thread::sleep_for(100ms);
        }
    }

    void startClient()
    {
        MyContextHost h{27016};


        while(h.busy())
        {
            std::this_thread::sleep_for(200ms);

            LoginRequest r;
            r.user() = "username";
            r.pass() = "passwrd";

            nl::Impl::PayloadTarget myself(
                nl::IpAddr::getLocalAddress(), 27015);


            h.send<LoginRequest>(myself, r);
        }
    }
}

int main()
{
    ssvu::lo("Choose") << "\n"
                       << "0. Server\n"
                       << "1. Client\n"
                       << "_. Exit\n";

    auto choice(getInput<int>("Choice"));

    if(choice == 0)
    {
        // choiceServer();
        example::startServer();
        NL_DEBUGLO() << "end choiceserver";
    }
    else if(choice == 1)
    {
        example::startClient();
        // choiceClient();
    }
    else
    {
        std::terminate();
    }

    NL_DEBUGLO() << "return0 ";
    return 0;
}
