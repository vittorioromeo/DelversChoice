#pragma once

#include "../include/NetLayer/NetLayer.hpp"

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
