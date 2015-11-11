#define EXAMPLE_USE_UDP 0

#include "../include/NetLayer/NetLayer.hpp"
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>
#include "../sql/ddl_definition.h"

#include "./input_utils.hpp"
#include "./example_aliases.hpp"
#include "./example_db.hpp"
#include "./example_pckt_defs.hpp"
#include "./example_utils.hpp"
#include "./example_db_actions.hpp"
#include "./example_bind.hpp"
#include "./example_server.hpp"
#include "./example_client.hpp"

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
#if EXAMPLE_USE_UDP
        std::cout << "Port?:\n";
        auto port(getInput<nl::Port>("Port"));

        example::startClient(port);
#else
        example::startClient(0);
#endif
    }
    else
    {
        std::terminate();
    }

    return 0;
}
