#include "../include/NetLayer/NetLayer.hpp"
// #include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"

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

    server.emplace_busy_loop([&server, &fnProcess]
        {
            server.try_process(fnProcess);
        });


    // auto server(nl::mkManagedHost(27015, fnProcess));
    // auto server(nl::makeManagedHost(27015, fnProcess));

    int cycles{20};

    while(server.isBusy())
    {

        if(getInput<int>("Exit? (1)")) break;

        // ::nl::debugLo() << "bsy";
        if(cycles-- <= 0)
        {
            // server.stop();
        }

        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // ssvu::lo() << "...\n";

        // if(processedCount > 0)
        //    ssvu::lo() << "Processed packets: " << processedCount << "\n\n";
    }

    ::nl::debugLo() << "serverend\n";
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

    client.emplace_busy_loop([&client, &fnProcess]
        {
            client.try_process(fnProcess);
        });


    while(client.isBusy())
    {

        std::this_thread::sleep_for(std::chrono::milliseconds(100));



        nl::PAddress myself(nl::IpAddr::getLocalAddress(), 27015);
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
        std::vector<std::pair<std::string, std::string>> regs;

        auto has_user([&](const auto& xuser)
            {
                return std::any_of(std::begin(regs), std::end(regs),
                    [&](const auto& x)
                    {
                        return std::get<0>(x) == xuser;
                    });
            });

        auto get_user([&](const auto& xuser)
            {
                for(const auto& p : regs)
                    if(std::get<0>(p) == xuser) return p;

                SSVU_UNREACHABLE();
            });

        MyContextHost h{27015};

        h.on_d<RegistrationRequest>(
            [&](const auto& sender, const auto& user, const auto& pass)
            {
                ssvu::lo() << "registration request from " << user
                           << ", pass: " << pass << "\n";

                if(!has_user(user))
                {
                    ssvu::lo() << "Replying with OK\n";
                    regs.emplace_back(user, pass);

                    h.send<RegistrationResponse>(sender, true);
                }
                else
                {
                    ssvu::lo() << "User already exists, replying with NO\n";

                    h.send<RegistrationResponse>(sender, false);
                }
            });

        h.on_d<LoginRequest>(
            [&](const auto& sender, const auto& user, const auto& pass)
            {
                ssvu::lo() << "login request from " << user
                           << ", pass: " << pass << "\n";

                if(!has_user(user))
                {
                    ssvu::lo() << "No such user.\nReplying with NO\n";
                    return;
                }

                const auto& u(get_user(user));

                if(std::get<1>(u) == pass)
                {
                    ssvu::lo() << "Replying with OK\n";

                    h.send<LoginResponse>(sender, true);
                }
                else
                {
                    ssvu::lo() << "Wrong password.\nReplying with NO\n";

                    h.send<LoginResponse>(sender, false);
                }
            });

        while(h.busy())
        {
            std::this_thread::sleep_for(100ms);
        }
    }

    void startClient()
    {
        MyContextHost h{27016};

        nl::PAddress serveraddr(
            nl::IpAddr::getLocalAddress(), 27015);

        h.on_d<RegistrationResponse>([&](const auto&, const auto& outcome)
            {
                ssvu::lo() << "Received registration response, outcome:\n"
                           << (outcome ? "VALID" : "INVALID") << "\n";
            });

        h.on_d<LoginResponse>([&](const auto&, const auto& outcome)
            {
                ssvu::lo() << "Received login response, outcome:\n"
                           << (outcome ? "VALID" : "INVALID") << "\n";
            });

        while(h.busy())
        {
            std::this_thread::sleep_for(200ms);


            ssvu::lo("Choose") << "\n"
                               << "0. Register\n"
                               << "1. Login\n"
                               << "_. Exit\n";

            auto choice(getInput<int>("Choice"));

            if(choice == 0 || choice == 1)
            {
                std::string username, password;

                ssvu::lo() << "Insert username:\n";
                std::cin >> username;

                ssvu::lo() << "Insert password:\n";
                std::cin >> password;

                if(choice == 0)
                {
                    h.make_and_send<RegistrationRequest>(
                        serveraddr, username, password);
                }
                else if(choice == 1)
                {
                    h.make_and_send<LoginRequest>(
                        serveraddr, username, password);
                }
            }
            else
            {
                h.stop();
                break;
            }
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
        ::nl::debugLo() << "end choiceserver";
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

    ::nl::debugLo() << "return0 ";
    return 0;
}
