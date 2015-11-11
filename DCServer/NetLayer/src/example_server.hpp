#pragma once

#include "../include/NetLayer/NetLayer.hpp"

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
}
