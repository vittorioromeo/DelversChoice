#pragma once

#include "../include/NetLayer/NetLayer.hpp"

namespace example
{
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
