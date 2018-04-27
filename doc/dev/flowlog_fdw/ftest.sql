/*
create extension flowlog_fdw;
create server flowlog_db foreign data wrapper flowlog_fdw options (nameText1 'valueText1', nameText2 'valueText2');
create user mapping for postgres server flowlog_db;

create foreign table flowlog_tb
(
        v1      integer,
        v2      text,
        v3      integer
) server flowlog_db;


select * from flowlog_tb;
select v3,v2 from flowlog_tb where v1 = 11;
insert into flowlog_tb (v1, v2, v3) values (31, 32, 33);
delete from flowlog_tb where v2 = '22';
update flowlog_tb set v2 = '22x' where v3 = 23;

drop foreign table flowlog_tb;
drop user mapping for postgres server flowlog_db;
drop server flowlog_db;
drop extension flowlog_fdw;


drop extension flowlog_fdw cascade;
*/


