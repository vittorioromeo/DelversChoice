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