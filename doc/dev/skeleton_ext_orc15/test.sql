
drop extension ext_orc15;
create extension ext_orc15;

select ext_orc_scan('/home/postgres/projects/filikadb/doc/dev/skeleton_ext_orc/TestOrcFile.test1.orc');
select ext_orc_content('/home/postgres/projects/filikadb/doc/dev/skeleton_ext_orc/TestOrcFile.test1.orc');
select ext_orc_mem_test('');



