// TODO: ???
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

    exec_choice( // .
        "Server",
        []
        {
            example::startServer();
        }, // .
        "Client",
        []
        {
#if EXAMPLE_USE_UDP
            example::startClient(ask_input<nl::Port>("port"));
#else
            example::startClient(0);
#endif
        }, // .
        "Exit",
        []
        {
            std::terminate();
        });

    return 0;
}

// TODO: check slides before exam
