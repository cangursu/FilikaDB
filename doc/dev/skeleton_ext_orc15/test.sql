
drop extension ext_orc15 cascade;
create extension ext_orc15 version '0.0.3';

select ext_orc_scan('/home/postgres/projects/filikadb/doc/dev/skeleton_ext_orc/TestOrcFile.test1.orc');
select ext_orc_content('/home/postgres/projects/filikadb/doc/dev/skeleton_ext_orc/TestOrcFile.test1.orc');
select ext_orc_mem_test('/home/postgres/sock_ext_orc15');



