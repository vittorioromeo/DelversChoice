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

        NL_DEFINE_PCKT(Notify,                   // .
            (                                    // .
                           ((int), channel_id),  // .
                           ((std::string), msg)) // .
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

        if(result.empty()) return false;

        f(result.front());
        return true;
    }

    template <typename TF>
    bool user_by_username(const std::string& username, TF&& f)
    {
        auto result(db()(select(all_of(tbl_user))
                             .from(tbl_user)
                             .where(tbl_user.username == username)));

        if(result.empty()) return false;

        f(result.front());
        return true;
    }

    bool has_user_by_id(int id)
    {
        return user_by_id(id, [](const auto&)
            {
            });
    }

    bool has_user_by_username(const std::string& username)
    {
        return user_by_username(username, [](const auto&)
            {
            });
    }

    template <typename TF>
    bool channel_by_id(int id, TF&& f)
    {
        auto result(db()(select(all_of(tbl_channel))
                             .from(tbl_channel)
                             .where(tbl_channel.id == id)));

        if(result.empty()) return false;

        f(result.front());
        return true;
    }

    bool has_channel_by_id(int id)
    {
        return channel_by_id(id, [](const auto&)
            {
            });
    }

    auto create_user(const std::string& user, const std::string& pass)
    {
        return db()(insert_into(tbl_user).set(tbl_user.username = user,
            tbl_user.pwdHash = utils::hash_pwd(pass)));
    }

    auto create_channel(int user_id, const std::string& name)
    {
        return db()(
            insert_into(tbl_channel)
                .set(tbl_channel.idUser = user_id, tbl_channel.name = name));
    }

    auto delete_channel(int id)
    {
        assert(has_channel_by_id(id));
        return db()(remove_from(tbl_channel).where(tbl_channel.id == id));
    }

    auto add_user_to_channel(int user_id, int channel_id)
    {
        assert(has_channel_by_id(user_id));
        assert(has_user_by_id(channel_id));

        return db()(insert_into(tbl_user_channel)
                        .set(tbl_user_channel.idUser = user_id,
                            tbl_user_channel.idChannel = channel_id));
    }

    auto remove_user_from_channel(int user_id, int channel_id)
    {
        assert(has_channel_by_id(user_id));
        assert(has_user_by_id(channel_id));

        return db()(remove_from(tbl_user_channel)
                        .where(tbl_user_channel.idUser == user_id &&
                               tbl_user_channel.idChannel == channel_id));
    }

    auto create_message(
        int user_id, int channel_id, const std::string& contents)
    {
        assert(has_channel_by_id(user_id));
        assert(has_user_by_id(channel_id));

        return db()(insert_into(tbl_message)
                        .set(tbl_message.idUser = user_id,
                            tbl_message.idChannel = channel_id,
                            tbl_message.contents = contents));
    }
}

int main()
{
    initialize_db_connection();

    // auto x =
    // db_actions::registration(example::to_s::Registration{nl::init_fields{},
    // "ciao", "Mondo"});
    // std::cout << x <<"\n";

    return 0;
}

namespace example
{
    namespace nle = experiment;

    using MySettings = nle::Settings<nl::UInt32>;

    constexpr auto my_pckt_binds(
        nle::pckt_binds<to_s::Registration, to_s::Login, to_s::CreateChannel,
            to_s::DeleteChannel, to_s::SendMessage, to_s::GetMessages,
            to_c::Outcome, to_c::Messages, to_c::Notify>());

    constexpr auto my_config(nle::make_config<MySettings>(my_pckt_binds));

    using MyConfig = decltype(my_config);

    using MyContextHost = nle::ContextHost<MyConfig>;
}

namespace example
{
    class connection_state
    {
    private:
        static constexpr int max_life{100};
        nl::PAddress _addr;
        int _life{max_life};
        int _id{0};

    public:
        connection_state(const nl::PAddress& addr) : _addr(addr) {}

        const auto& addr() { return _addr; }
        const auto& id() { return _id; }

        auto is_logged_in() { return _id > 0; }
        auto dead() { return _life <= 0; }

        void decrease_life() { --_life; }
        void reset_life() { _life = max_life; }
        void set_logged_in_as(int id)
        {
            assert(!is_logged_in());
            _id = id;
        }
    };

    class server_state
    {
    private:
        std::vector<std::unique_ptr<connection_state>> _connections;

    public:
        server_state() = default;

        connection_state* conn_by_addr(const nl::PAddress& x)
        {
            for(auto& c : _connections)
                if(c->addr() == x) return c.get();

            return nullptr;
        }

        connection_state* conn_by_id(int id)
        {
            for(auto& c : _connections)
                if(c->id() == id) return c.get();

            return nullptr;
        }

        void decrease_life()
        {
            for(auto& c : _connections) c->decrease_life();

            ssvu::eraseRemoveIf(_connections, [](const auto& x)
                {
                    if(x->dead())
                    {
                        ssvu::lo() << "Client #" << x._id << " timed out.\n";
                        return true;
                    }

                    return false;
                });
        }
    };
}

namespace example
{
    void startServer()
    {
        using namespace to_s;

        server_state s;
        MyContextHost h{27015};

        h.on_d<Registration>(
            [&](const auto& sender, const auto& user, const auto& pass)
            {
            });

        h.on_d<Login>(
            [&](const auto& sender, const auto& user, const auto& pass)
            {
            });

        h.on_d<CreateChannel>([&](const auto& sender, const auto& name)
            {
            });

        h.on_d<DeleteChannel>([&](const auto& sender, const auto& id)
            {
            });

        h.on_d<SendMessage>([&](
            const auto& sender, const auto& channel_id, const auto& contents)
            {
            });

        h.on_d<GetMessages>(
            [&](const auto& sender, const auto& channel_id, const auto& count)
            {
            });

        while(h.busy())
        {
            // Try to process all packets.
            while(h.try_dispatch_and_process())
            {
            }

            // Timeout.
            s.decrease_life();

            std::this_thread::sleep_for(100ms);
        }
    }

    enum class cs
    {
        unlogged,
        logged,
        awaiting_channel_list,
        selecting_channel,
        awaiting_login_response,
        awaiting_create_channel_response,
        awaiting_registration_response,
    };

    struct client_state
    {
        cs s{cs::unlogged};
    };

    void startClient()
    {
        using namespace to_c;

        MyContextHost h{27016};
        client_state s;

        nl::PAddress serveraddr(nl::IpAddr::getLocalAddress(), 27015);

        h.on_d<Outcome>([&](const auto&, const auto& valid, const auto& msg)
            {
                ssvu::lo() << "Received registration response, outcome:\n"
                           << (valid ? "VALID" : "INVALID")
                           << "\nMessage: " << msg << "\n\n";
            });

        h.on_d<Messages>([&](const auto&, const auto& messages)
            {

            });

        h.on_d<Notify>([&](const auto&, const auto& channel_id, const auto& msg)
            {

            });

        while(h.busy())
        {
            // Try to process all packets.
            while(h.try_dispatch_and_process())
            {
            }

            // std::cout << "NO MORE P...\n";

            if(s.s == cs::awaiting_login_response)
            {
                ssvu::lo() << "Awaiting login response...\n";
            }
            else if(s.s == cs::awaiting_create_channel_response)
            {
                ssvu::lo() << "Awaiting create channel response...\n";
            }
            else if(s.s == cs::awaiting_registration_response)
            {
                ssvu::lo() << "Awaiting registration response...\n";
            }
            else if(s.s == cs::unlogged)
            {
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
                        h.make_and_send<to_s::Registration>(
                            serveraddr, username, password);

                        s.s = cs::awaiting_registration_response;
                    }
                    else if(choice == 1)
                    {
                        h.make_and_send<to_s::Login>(
                            serveraddr, username, password);

                        s.s = cs::awaiting_login_response;
                    }
                }
                else
                {
                    h.stop();
                    break;
                }
            }
            else if(s.s == cs::logged)
            {
                ssvu::lo("Choose") << "\n"
                                   << "0. Create channel\n"
                                   << "1. Subscribe to channel\n"
                                   << "2. Send broadcast\n"
                                   << "_. Logout\n";

                auto choice(getInput<int>("Choice"));

                if(choice == 0)
                {
                    auto name = getInputLine("Channel name:");
                    h.make_and_send<to_s::CreateChannel>(serveraddr, name);
                    s.s = cs::awaiting_create_channel_response;
                }
                else if(choice == 1)
                {
                    h.make_and_send<to_s::ChannelList>(serveraddr);
                    s.s = cs::awaiting_channel_list;
                }
                else if(choice == 2)
                {
                }
                else
                {
                    h.make_and_send<to_s::Logout>(serveraddr);
                }
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
