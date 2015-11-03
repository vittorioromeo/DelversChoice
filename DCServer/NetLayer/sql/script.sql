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




################################################################################
# TABLE
# * This table deals with users. 
################################################################################
create table tbl_user
(
	# Primary key
	id int auto_increment primary key,

	# Credentials
	username varchar(255) not null,
	pwd_hash varchar(255) not null
)$
################################################################################



################################################################################
# TABLE
# * This table deals with channels. 
################################################################################
create table tbl_channel
(
	# Primary key
	id int auto_increment primary key,

	# Information
	name varchar(255) not null,

	# Author
	id_user int not null,

	foreign key (id_user)
		references tbl_user(id)
		on update cascade
		on delete cascade
)$
################################################################################



################################################################################
# TABLE
# * N-N relationship between users and channels.
################################################################################
create table tbl_user_channel
(
	# Primary key
	id int auto_increment primary key,

	# Foreign keys
	id_user int not null,
	id_channel int not null,

	foreign key (id_user)
		references tbl_user(id)
		on update cascade
		on delete cascade,

	foreign key (id_channel)
		references tbl_channel(id)
		on update cascade
		on delete cascade
)$
################################################################################



################################################################################
# TABLE
# * Represents a message.
################################################################################
create table tbl_message
(
	# Primary key
	id int auto_increment primary key,

	# Data
	contents text not null,
	# creation_date date not null,

	# Author
	id_user int not null,

	# Channel
	id_channel int not null,

	foreign key (id_user)
		references tbl_user(id)
		on update cascade
		on delete cascade,

	foreign key (id_channel)
		references tbl_channel(id)
		on update cascade
		on delete cascade
)$
################################################################################



create procedure initialize_db_netlayer_example()
begin
	insert into tbl_user(username, pwd_hash)
		values("admin", "admin");

	insert into tbl_channel(name, id_user)
		values("welcome", 1);
end$

call initialize_db_netlayer_example()$



