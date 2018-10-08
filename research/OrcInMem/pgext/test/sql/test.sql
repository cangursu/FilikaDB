--select orc_buffer_test('/home/postgres/.sock_domain_log');
--select orc_inmem_test('/home/postgres/.sock_domain_log', '/home/postgres/projects/filikadb/research/OrcInMem/pgext/test/data/');
select orc_buffer_remote_test('/home/postgres/.sock_domain_log',  '/home/postgres/.sock_domain_pgext');
