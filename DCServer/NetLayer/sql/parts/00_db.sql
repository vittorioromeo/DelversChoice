################################################################################
# Copyright (c) 2013-2015 Vittorio Romeo
# License: Academic Free License ("AFL") v. 3.0
# AFL License page: http://opensource.org/licenses/AFL-3.0
################################################################################
# http://vittorioromeo.info
# vittorio.romeo@outlook.com
################################################################################

################################################################################
# NetLayer example application database initialization and creation script
################################################################################

################################################################################
# This script is meant to be run once to create and initializea
# from scratch the whole example database.
# Therefore, we drop the database if exists and re-create it.
drop database if exists db_netlayer_example$
create database db_netlayer_example$
use db_netlayer_example$
################################################################################
