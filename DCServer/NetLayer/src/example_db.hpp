#pragma once

#include "../include/NetLayer/NetLayer.hpp"
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>
#include "../sql/ddl_definition.h"
#include "./example_aliases.hpp"

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
