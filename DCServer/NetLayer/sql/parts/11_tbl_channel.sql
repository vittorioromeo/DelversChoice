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