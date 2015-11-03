create procedure initialize_db_netlayer_example()
begin
	insert into tbl_user(username, pwd_hash)
		values("admin", "admin");

	insert into tbl_channel(name, id_user)
		values("welcome", 1);
end$

call initialize_db_netlayer_example()$