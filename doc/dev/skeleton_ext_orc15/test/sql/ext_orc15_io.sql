-- simple input
SELECT '120'::ext_orc15;
SELECT '3c'::ext_orc15;
-- case insensitivity
SELECT '3C'::ext_orc15;
SELECT 'FoO'::ext_orc15;
-- invalid characters
SELECT 'foo bar'::ext_orc15;
SELECT 'abc$%2'::ext_orc15;
-- negative values
SELECT '-10'::ext_orc15;
-- to big values
SELECT 'abcdefghi'::ext_orc15;

-- storage
BEGIN;
CREATE TABLE ext_orc15_test(val ext_orc15);
INSERT INTO ext_orc15_test VALUES ('123'), ('3c'), ('5A'), ('zZz');
SELECT * FROM ext_orc15_test;
UPDATE ext_orc15_test SET val = '567a' where val = '123';
SELECT * FROM ext_orc15_test;
UPDATE ext_orc15_test SET val = '-aa' where val = '3c';
SELECT * FROM ext_orc15_test;
ROLLBACK;
