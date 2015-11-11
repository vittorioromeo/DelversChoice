#pragma once

#include "../include/NetLayer/NetLayer.hpp"


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
    constexpr auto my_server_tunnel(
        nle::tunnel_type<nl::Tunnel::TCPListener>{});
    constexpr auto my_client_tunnel(nle::tunnel_type<nl::Tunnel::TCPSckt>{});

#endif

    constexpr auto my_server_config(
        nle::make_config<MySettings>(my_pckt_binds, my_server_tunnel));
    constexpr auto my_client_config(
        nle::make_config<MySettings>(my_pckt_binds, my_client_tunnel));

    using MyServerConfig = decltype(my_server_config);
    using MyClientConfig = decltype(my_client_config);

    using MyCtxServer = nle::ContextHost<MyServerConfig>;
    using MyCtxClient = nle::ContextHost<MyClientConfig>;
}
