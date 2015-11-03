#include "../include/NetLayer/NetLayer.hpp"
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>
#include "../sql/ddl_definition.h"

namespace mysql = sqlpp::mysql;
namespace ddl = example_ddl;

template <typename T>
auto getInput(const std::string& title)
{
    T input;
    std::cin >> input;

    ssvu::lo(title) << input << "\n";
    return input;
}

auto getInputLine(const std::string& title)
{
    std::string input;
    getline(std::cin, input);

    ssvu::lo(title) << input << "\n";
    return input;
}

std::unique_ptr<mysql::connection> _db;
example_ddl::TblUser tbl_user;
example_ddl::TblChannel tbl_channel;
example_ddl::TblMessage tbl_message;
example_ddl::TblUserChannel tbl_user_channel;

auto& db()
{
    assert(_db != nullptr);
    return *_db;
}

void initialize_db_connection()
{
    auto config = std::make_shared<mysql::connection_config>();

    config->host = "127.0.0.1";
    config->user = "userx";
    config->password = "root";
    config->port = 3306;
    config->database = "db_netlayer_example";
    config->debug = true;

    _db = std::make_unique<mysql::connection>(config);
}

namespace example
{
    namespace to_s
    {
        NL_DEFINE_PCKT(Registration,              // .
            (                                     // .
                           ((std::string), user), // .
                           ((std::string), pass)) // .
            );

        NL_DEFINE_PCKT(Login,                     // .
            (                                     // .
                           ((std::string), user), // .
                           ((std::string), pass)) // .
            );

        NL_DEFINE_PCKT_1(CreateChannel,           // .
            (                                     // .
                             (std::string), name) // .
            );

        NL_DEFINE_PCKT_1(DeleteChannel, // .
            (                           // .
                             (int), id) // .
            );

        NL_DEFINE_PCKT(SendMessage,                   // .
            (                                         // .
                           ((int), channel_id),       // .
                           ((std::string), contents)) // .
            );

        NL_DEFINE_PCKT(GetMessages,             // .
            (                                   // .
                           ((int), channel_id), // .
                           ((int), count))      // .
            );
    }

    namespace to_c
    {
        NL_DEFINE_PCKT(Outcome,                  // .
            (                                    // .
                           ((bool), valid),      // .
                           ((std::string), msg)) // .
            );

        NL_DEFINE_PCKT_1(Messages,                                 // .
            (                                                      // .
                             (std::vector<std::string>), messages) // .
            );
    }
}

namespace utils
{
    auto hash_pwd(const std::string& x) { return x; }
}

namespace db_actions
{
    using namespace example;
    using namespace example_ddl;

    template <typename TF>
    bool user_by_id(int id, TF&& f)
    {
        auto result(db()(
            select(all_of(tbl_user)).from(tbl_user).where(tbl_user.id == id)));

        if(result.empty())
        {
            return false;
        }

        f(result.front());
        return true;
    }

    bool has_user_by_id(int id)
    {
        return user_by_id(id, [](const auto&)
            {
            });
    }

    auto registration(const to_s::Registration& x)
    {
        return db()(insert_into(tbl_user).set(tbl_user.username = x.user(),
            tbl_user.pwdHash = utils::hash_pwd(x.pass())));
    }
}

int main()
{
    initialize_db_connection();

    return 0;
}


namespace example
{
    NL_DEFINE_PCKT(
        RegistrationRequest, (((std::string), user), ((std::string), pass)));

    NL_DEFINE_PCKT_1(RegistrationResponse, ((bool), valid));

    NL_DEFINE_PCKT(
        LoginRequest, (((std::string), user), ((std::string), pass)));

    NL_DEFINE_PCKT_1(LoginResponse, ((bool), valid));
    NL_DEFINE_PCKT_1(Message, ((std::string), msg));

    namespace nle = experiment;

    using MySettings = nle::Settings<nl::UInt32>;

    constexpr auto my_pckt_binds(nle::pckt_binds<RegistrationRequest,
        LoginRequest, RegistrationResponse, LoginResponse, Message>());

    constexpr auto my_config(nle::make_config<MySettings>(my_pckt_binds));

    using MyConfig = decltype(my_config);

    using MyContextHost = nle::ContextHost<MyConfig>;

    static_assert(MyConfig::getPcktBindID<RegistrationRequest>() == 0, "");
    static_assert(MyConfig::getPcktBindID<LoginRequest>() == 1, "");
    static_assert(MyConfig::getPcktBindID<RegistrationResponse>() == 2, "");
    static_assert(MyConfig::getPcktBindID<LoginResponse>() == 3, "");


    void startServer()
    {
        struct user_data
        {
            nl::PAddress _id;
            std::string _user;
            std::string _pass;

            user_data(const nl::PAddress& id, const std::string& user,
                const std::string& pass)
                : _id(id), _user(user), _pass(pass)
            {
            }
        };

        struct login_data
        {
            nl::PAddress _id;
            int _life{100};

            login_data(const nl::PAddress& id) : _id(id) {}
        };

        // int next_id = 0;
        std::vector<user_data> registered_users;
        std::vector<login_data> logged_in;

        auto user_exists([&](const auto& xuser)
            {
                for(const auto& x : registered_users)
                    if(x._user == xuser) return true;

                return false;
            });

        auto get_user_by_username([&](const auto& xuser)
            {
                for(const auto& x : registered_users)
                    if(x._user == xuser) return x;

                SSVU_UNREACHABLE();
            });

        auto is_logged_in([&](const auto& id)
            {
                for(const auto& x : logged_in)
                    if(x._id == id) return true;

                return false;
            });

        auto get_logged_user_by_id([&](const auto& id) -> login_data&
            {
                for(auto& x : logged_in)
                    if(x._id == id) return x;

                SSVU_UNREACHABLE();
            });

        MyContextHost h{27015};

        h.on_d<RegistrationRequest>(
            [&](const auto& sender, const auto& user, const auto& pass)
            {
                ssvu::lo() << "registration request from " << user
                           << ", pass: " << pass << "\n";

                if(!user_exists(user))
                {
                    ssvu::lo() << "Replying with OK\n";
                    registered_users.emplace_back(sender, user, pass);

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

                if(!user_exists(user))
                {
                    ssvu::lo() << "No such user.\nReplying with NO\n";
                    return;
                }

                const auto& u(get_user_by_username(user));

                if(u._pass == pass)
                {
                    if(is_logged_in(sender))
                    {
                        ssvu::lo() << "Already logged in.\nReplying with NO\n";
                        h.send<LoginResponse>(sender, false);
                    }
                    else
                    {
                        ssvu::lo() << "Replying with OK\n";
                        logged_in.emplace_back(u._id);
                        h.send<LoginResponse>(sender, true);
                    }
                }
                else
                {
                    ssvu::lo() << "Wrong password.\nReplying with NO\n";

                    h.send<LoginResponse>(sender, false);
                }
            });

        h.on_d<Message>([&](const auto& sender, const auto& msg)
            {
                if(!is_logged_in(sender))
                {
                    ssvu::lo() << "Error: not logged in.\n";
                    return;
                }

                get_logged_user_by_id(sender)._life = 100;

                ssvu::lo() << "Message from " << sender << ":\n" << msg
                           << "\n\n";
            });

        while(h.busy())
        {
            // Try to process all packets.
            while(h.try_dispatch_and_process())
            {
                // std::cout << "p...\n";
            }

            //   std::cout << "NO MORE P...\n";

            for(auto& x : logged_in)
            {
                --x._life;
            }

            ssvu::eraseRemoveIf(logged_in, [](const auto& x)
                {
                    if(x._life <= 0)
                    {
                        ssvu::lo() << "Client #" << x._id << " timed out.\n";
                        return true;
                    }

                    return false;
                });

            std::this_thread::sleep_for(100ms);
        }
    }

    void startClient()
    {
        MyContextHost h{27016};

        nl::PAddress serveraddr(nl::IpAddr::getLocalAddress(), 27015);

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
            // Try to process all packets.
            while(h.try_dispatch_and_process())
            {
                // std::cout << "p...\n";
            }

            // std::cout << "NO MORE P...\n";



            ssvu::lo("Choose") << "\n"
                               << "0. Register\n"
                               << "1. Login\n"
                               << "2. Send message\n"
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
            else if(choice == 2)
            {
                std::string m;
                ssvu::lo() << "Insert message:\n";
                std::cin >> m;

                h.make_and_send<Message>(serveraddr, m);
            }
            else
            {
                h.stop();
                break;
            }

            std::this_thread::sleep_for(100ms);
        }
    }
}

int main_xd()
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
