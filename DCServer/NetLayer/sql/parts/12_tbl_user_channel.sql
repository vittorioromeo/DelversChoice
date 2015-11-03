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