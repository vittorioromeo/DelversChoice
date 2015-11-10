#include "../include/NetLayer/NetLayer.hpp"
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>
#include "../sql/ddl_definition.h"

#define EXAMPLE_USE_UDP 0

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
    std::getline(std::cin, input);

    // ssvu::lo(title) << input << "\n";
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
    config->user = "root";
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

        NL_DEFINE_PCKT_1(Subscribe,     // .
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

        NL_DEFINE_PCKT_0(ChannelList);
        NL_DEFINE_PCKT_0(Logout);
    }

    namespace to_c
    {
        constexpr int ot_login = 0;
        constexpr int ot_registration = 1;
        constexpr int ot_create_channel = 2;
        constexpr int ot_create_message = 3;
        constexpr int ot_subscribe = 4;

        NL_DEFINE_PCKT(Outcome,             // .
            (                               // .
                           ((bool), valid), // .
                           ((int), type))   // .
            );

        NL_DEFINE_PCKT_1(Messages,                                 // .
            (                                                      // .
                             (std::vector<std::string>), messages) // .
            );

        NL_DEFINE_PCKT_1(Channels,                                 // .
            (                                                      // .
                             (std::vector<std::string>), channels) // .
            );

        NL_DEFINE_PCKT(Notify,                   // .
            (                                    // .
                           ((int), channel_id),  // .
                           ((std::string), msg)) // .
            );

        NL_DEFINE_PCKT_0(TimedOut);
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

    namespace impl
    {
        template <typename T, typename TF>
        bool execute_if_not_empty(T&& r, TF&& f)
        {
            if(r.empty()) return false;

            f(r.front());
            return true;
        }
    }

    template <typename TF>
    void for_channels(TF&& f)
    {
        auto result(
            db()(select(all_of(tbl_channel)).from(tbl_channel).where(true)));

        for(const auto& row : result) f(row);
    }

    template <typename TF>
    bool message_by_id(int id, TF&& f)
    {
        auto result(db()(select(all_of(tbl_message))
                             .from(tbl_message)
                             .where(tbl_message.id == id)));

        return impl::execute_if_not_empty(result, f);
    }

    template <typename TF>
    bool user_by_id(int id, TF&& f)
    {
        auto result(db()(
            select(all_of(tbl_user)).from(tbl_user).where(tbl_user.id == id)));

        return impl::execute_if_not_empty(result, f);
    }

    template <typename TF>
    bool user_by_username(const std::string& username, TF&& f)
    {
        auto result(db()(select(all_of(tbl_user))
                             .from(tbl_user)
                             .where(tbl_user.username == username)));

        return impl::execute_if_not_empty(result, f);
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

        return impl::execute_if_not_empty(result, f);
    }

    template <typename TF>
    bool channel_by_name(const std::string& name, TF&& f)
    {
        auto result(db()(select(all_of(tbl_channel))
                             .from(tbl_channel)
                             .where(tbl_channel.name == name)));

        return impl::execute_if_not_empty(result, f);
    }

    bool has_channel_by_id(int id)
    {
        return channel_by_id(id, [](const auto&)
            {
            });
    }

    bool has_channel_by_name(const std::string& name)
    {
        return channel_by_name(name, [](const auto&)
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
        assert(has_channel_by_id(channel_id));
        assert(has_user_by_id(user_id));

        return db()(insert_into(tbl_user_channel)
                        .set(tbl_user_channel.idUser = user_id,
                            tbl_user_channel.idChannel = channel_id));
    }

    auto remove_user_from_channel(int user_id, int channel_id)
    {
        assert(has_channel_by_id(channel_id));
        assert(has_user_by_id(user_id));

        return db()(remove_from(tbl_user_channel)
                        .where(tbl_user_channel.idUser == user_id &&
                               tbl_user_channel.idChannel == channel_id));
    }

    auto create_message(
        int user_id, int channel_id, const std::string& contents)
    {
        assert(has_channel_by_id(channel_id));
        assert(has_user_by_id(user_id));

        return db()(insert_into(tbl_message)
                        .set(tbl_message.idUser = user_id,
                            tbl_message.idChannel = channel_id,
                            tbl_message.contents = contents));
    }

    auto is_user_in_channel(int user_id, int channel_id)
    {
        auto result(db()(select(all_of(tbl_user_channel))
                             .from(tbl_user_channel)
                             .where(tbl_user_channel.idUser == user_id &&
                                    tbl_user_channel.idChannel == channel_id)));

        return !result.empty();
    }

    template <typename TF>
    void for_users_subscribed_to(int channel_id, TF&& f)
    {
        if(!has_channel_by_id(channel_id)) return;

        auto result(db()(select(all_of(tbl_user_channel))
                             .from(tbl_user_channel)
                             .where(tbl_user_channel.idChannel == channel_id)));

        for(const auto& row : result) f(row.idUser);
    }
}


namespace example
{
    namespace nle = experiment;

    using MySettings = nle::Settings<nl::UInt32>;

    constexpr auto my_pckt_binds(
        nle::pckt_binds<to_s::Registration, to_s::Login, to_s::CreateChannel,
            to_s::DeleteChannel, to_s::SendMessage, to_s::GetMessages,
            to_s::ChannelList, to_s::Subscribe, to_s::Logout, to_c::Outcome,
            to_c::Messages, to_c::Notify, to_c::Channels, to_c::TimedOut>());

#if EXAMPLE_USE_UDP
    constexpr auto my_server_tunnel(nle::tunnel_type<nl::Tunnel::UDPSckt>{});
    constexpr auto my_client_tunnel(nle::tunnel_type<nl::Tunnel::UDPSckt>{});
#else
    constexpr auto my_server_tunnel(nle::tunnel_type<nl::Tunnel::TCPListener>{});
    constexpr auto my_client_tunnel(nle::tunnel_type<nl::Tunnel::TCPSckt>{});

#endif

    constexpr auto my_server_config(nle::make_config<MySettings>(my_pckt_binds, my_server_tunnel));
    constexpr auto my_client_config(nle::make_config<MySettings>(my_pckt_binds, my_client_tunnel));

    using MyServerConfig = decltype(my_server_config);
    using MyClientConfig = decltype(my_client_config);

    using MyCtxServer = nle::ContextHost<MyServerConfig>;
    using MyCtxClient = nle::ContextHost<MyClientConfig>;
}

namespace example
{
    class server_state;

    class connection_state
    {
        friend class server_state;

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

        template <typename... Ts>
        auto add_connection(Ts&&... xs)
        {
            _connections.emplace_back(
                std::make_unique<connection_state>(FWD(xs)...));

            return _connections.back().get();
        }

    public:
        server_state() = default;

        bool logout(const nl::PAddress& x)
        {
            auto c = conn_by_addr(x);
            if(c == nullptr) return false;

            ssvu::eraseRemoveIf(_connections, [&](const auto& y)
                {
                    return y.get() == c;
                });

            return true;
        }

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

        void connect_or_reset(int id, const nl::PAddress& x)
        {
            auto c = conn_by_addr(x);

            if(c == nullptr)
            {
                auto cc = add_connection(x);
                cc->_id = id;
            }
            else
            {
                c->reset_life();
                c->_id = id;
            }
        }

        template <typename TF>
        void decrease_life(TF&& f)
        {
            for(auto& c : _connections) c->decrease_life();

            ssvu::eraseRemoveIf(_connections, [&](const auto& x)
                {
                    if(!x->dead()) return false;

                    f(x->addr());
                    ssvu::lo() << "Client #" << x->id() << " timed out.\n";
                    return true;
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
        MyCtxServer h{27015};

#if EXAMPLE_USE_UDP
        h.try_bind_tunnel(27015);
#else
        h.try_bind_tunnel(27015);
#endif

        auto print_success([&](bool x, const auto& msg)
            {
                ssvu::lo() << (x ? "Success: "s : "Failure: "s) << msg;
            });

        auto execute_connected = [&](const auto& addr, auto&& f)
        {
            auto c = s.conn_by_addr(addr);
            if(c == nullptr) return false;

            f(c);
            return true;
        };

        h.on_d<Registration>(
            [&](const auto& sender, const auto& user, const auto& pass)
            {
                bool success = false;

                if(!db_actions::has_user_by_username(user))
                {
                    auto r = db_actions::create_user(user, pass);

                    success = r > 0;
                    print_success(success, "user "s + user + " registration\n");
                }

                h.try_make_and_send_pckt<to_c::Outcome>(
                    sender, to_c::ot_registration, success);
            });

        h.on_d<Login>(
            [&](const auto& sender, const auto& user, const auto& pass)
            {
                bool success = false;
                int uid = 0;

                if(db_actions::has_user_by_username(user))
                {
                    db_actions::user_by_username(user, [&](const auto& x)
                        {
                            success = x.pwdHash == utils::hash_pwd(pass);
                            uid = x.id;

                            if(success)
                            {
                                s.connect_or_reset(uid, sender);
                            }
                        });
                }

                print_success(success, "user "s + user + " logged in\n");
                h.try_make_and_send_pckt<to_c::Outcome>(
                    sender, success, to_c::ot_login);
            });

        h.on_d<CreateChannel>([&](const auto& sender, const auto& name)
            {
                bool success = execute_connected(sender, [&](auto& c)
                    {
                        if(db_actions::has_channel_by_name(name)) return false;

                        auto r = db_actions::create_channel(c->id(), name);
                        return r > 0;
                    });

                h.try_make_and_send_pckt<to_c::Outcome>(
                    sender, success, to_c::ot_create_channel);
            });

        /*
        h.on_d<DeleteChannel>([&](const auto& sender, const auto& id)
            {
            });
        */

        h.on_d<SendMessage>([&](
            const auto& sender, const auto& channel_id, const auto& contents)
            {
                int id = 0;
                bool success = execute_connected(sender, [&](auto& c)
                    {
                        if(!db_actions::has_channel_by_id(channel_id))
                            return false;

                        if(!db_actions::is_user_in_channel(c->id(), channel_id))
                            return false;

                        auto r = db_actions::create_message(
                            c->id(), channel_id, contents);

                        id = r;
                        return r > 0;
                    });

                h.try_make_and_send_pckt<to_c::Outcome>(
                    sender, success, to_c::ot_create_message);

                if(success && id > 0)
                {
                    db_actions::for_users_subscribed_to(channel_id,
                        [&](const auto& uid)
                        {
                            auto msg_id = id;
                            std::string msg;

                            db_actions::message_by_id(msg_id, [&](const auto& m)
                                {
                                    msg = m.contents;
                                });

                            std::ostringstream os;
                            os << "New message in channel " << channel_id
                               << ": " << msg << "\n";

                            h.try_make_and_send_pckt<to_c::Notify>(
                                s.conn_by_id(uid)->addr(), channel_id,
                                os.str());
                        });
                }
            });

        /*
        h.on_d<GetMessages>(
            [&](const auto& sender, const auto& channel_id, const auto& count)
            {
            });
        */

        h.on_d<Logout>([&](const auto& sender, auto)
            {
                s.logout(sender);
            });


        h.on_d<ChannelList>([&](const auto& sender, auto)
            {
                execute_connected(sender, [&](auto&)
                    {
                        std::vector<std::string> vec;
                        db_actions::for_channels([&](const auto& row)
                            {
                                std::string id_str = std::to_string(row.id);
                                std::string name_str = row.name;

                                vec.emplace_back(id_str + ": " + name_str);
                            });

                        h.try_make_and_send_pckt<to_c::Channels>(sender, vec);
                    });
            });

        h.on_d<Subscribe>([&](const auto& sender, const auto& channel_id)
            {
                bool success = execute_connected(sender, [&](auto& c)
                    {
                        if(!db_actions::has_channel_by_id(channel_id))
                            return false;

                        // TODO: check duplicate subscriptions
                        auto r = db_actions::add_user_to_channel(
                            c->id(), channel_id);

                        return r > 0;
                    });

                h.try_make_and_send_pckt<to_c::Outcome>(
                    sender, success, to_c::ot_subscribe);
            });

        while(h.busy())
        {
            // Try to process all packets.
            while(h.try_dispatch_and_process())
            {
            }

            // Timeout.
            s.decrease_life([&](const auto& addr)
                {
                    h.try_make_and_send_pckt<to_c::TimedOut>(addr);
                });

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
        awaiting_create_message_response,
        awaiting_subscribe_response,
    };

    struct client_state
    {
        cs s{cs::unlogged};
    };

    void startClient(nl::Port port)
    {
        using namespace to_c;

        nl::PAddress serveraddr(nl::IpAddr::getLocalAddress(), 27015);
        MyCtxClient h{port};

#if EXAMPLE_USE_UDP
        h.try_bind_tunnel(port);
#else
        h.try_bind_tunnel(serveraddr.ip, serveraddr.port);
#endif

        client_state s;


        h.on_d<Outcome>([&](const auto&, const auto& valid, const auto& otv)
            {
                switch(otv)
                {
                    case to_c::ot_registration: // .
                        s.s = cs::unlogged;
                        break;
                    case to_c::ot_create_channel: // .
                    case to_c::ot_create_message: // .
                    case to_c::ot_subscribe:      // .
                        s.s = cs::logged;
                        break;
                    case to_c::ot_login: // .
                        s.s = valid ? cs::logged : cs::unlogged;
                        break;
                }
            });

        /*
        h.on_d<Messages>([&](const auto&, const auto& messages)
            {

            });
            */

        h.on_d<TimedOut>([&](const auto&, const auto&)
            {
                s.s = cs::unlogged;
                std::cout << "Force-disconnected.\n";
            });

        h.on_d<Notify>([&](const auto&, const auto& channel_id, const auto& msg)
            {
                (void)channel_id;

                std::cout << "Notification:\n" << msg << "\n";
            });

        h.on_d<Channels>([&](const auto&, const auto& vec)
            {
                for(const auto& sx : vec)
                {
                    std::cout << sx << "\n";
                }

                std::cout << "\n";

                s.s = cs::logged;
            });

        while(h.busy())
        {
            // Try to process all packets.
            while(h.try_dispatch_and_process())
            {
            }

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
            else if(s.s == cs::awaiting_subscribe_response)
            {
                ssvu::lo() << "Awaiting subscribe response...\n";
            }
            else if(s.s == cs::awaiting_create_message_response)
            {
                ssvu::lo() << "Awaiting create message response...\n";
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
                        h.try_make_and_send_pckt<to_s::Registration>(
                            serveraddr, username, password);

                        s.s = cs::awaiting_registration_response;
                    }
                    else if(choice == 1)
                    {
                        h.try_make_and_send_pckt<to_s::Login>(
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
                                   << "1. Get channel list\n"
                                   << "2. Subscribe to channel\n"
                                   << "3. Send broadcast\n"
                                   << "_. Logout\n";

                auto choice(getInput<int>("Choice"));

                if(choice == 0)
                {
                    auto name = getInput<std::string>("Channel name:");
                    h.try_make_and_send_pckt<to_s::CreateChannel>(
                        serveraddr, name);
                    s.s = cs::awaiting_create_channel_response;
                }
                else if(choice == 1)
                {
                    h.try_make_and_send_pckt<to_s::ChannelList>(serveraddr);
                    s.s = cs::awaiting_channel_list;
                }
                else if(choice == 2)
                {
                    auto ch_id(getInput<int>("Channel ID"));
                    h.try_make_and_send_pckt<to_s::Subscribe>(
                        serveraddr, ch_id);
                    s.s = cs::awaiting_subscribe_response;
                }
                else if(choice == 3)
                {
                    auto ch_id(getInput<int>("Channel ID"));
                    auto msg = getInput<std::string>("Message:");
                    h.try_make_and_send_pckt<to_s::SendMessage>(
                        serveraddr, ch_id, msg);
                    s.s = cs::awaiting_create_message_response;
                }
                else
                {
                    h.try_make_and_send_pckt<to_s::Logout>(serveraddr);
                    s.s = cs::unlogged;
                }
            }

            std::this_thread::sleep_for(100ms);
        }
    }
}

int main()
{
    initialize_db_connection();

    ssvu::lo("Choose") << "\n"
                       << "0. Server\n"
                       << "1. Client\n"
                       << "_. Exit\n";

    auto choice(getInput<int>("Choice"));

    if(choice == 0)
    {
        example::startServer();
    }
    else if(choice == 1)
    {
        std::cout << "Port?:\n";
        auto port(getInput<nl::Port>("Port"));
        example::startClient(port);
    }
    else
    {
        std::terminate();
    }

    return 0;
}
