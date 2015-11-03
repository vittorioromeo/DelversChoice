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