CREATE EXTENSION ext_orc;
SELECT ext_orc_scan('/home/postgres/projects/filikadb/doc/dev/skeleton_ext_orc/TestOrcFile.test1.orc');
SELECT ext_orc_content('/home/postgres/projects/filikadb/doc/dev/skeleton_ext_orc/TestOrcFile.test1.orc');