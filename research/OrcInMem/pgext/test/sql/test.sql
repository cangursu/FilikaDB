--select orc_buffer_test('/home/postgres/sock_orcinmem');
--select orc_inmem_test('/home/postgres/sock_orcinmem', '/home/postgres/projects/filikadb/research/OrcInMem/pgext/test/sql/data/');
select orc_buffer_remote_test('/home/postgres/sock_orcinmem',  '/home/postgres/projects/filikadb/research/OrcInMem/MemStreamServer/remotesock');