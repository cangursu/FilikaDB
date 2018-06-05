-- comparison
SELECT '120'::ext_orc15 > '3c'::ext_orc15;
SELECT '120'::ext_orc15 >= '3c'::ext_orc15;
SELECT '120'::ext_orc15 < '3c'::ext_orc15;
SELECT '120'::ext_orc15 <= '3c'::ext_orc15;
SELECT '120'::ext_orc15 <> '3c'::ext_orc15;
SELECT '120'::ext_orc15 = '3c'::ext_orc15;

-- comparison equals
SELECT '120'::ext_orc15 > '120'::ext_orc15;
SELECT '120'::ext_orc15 >= '120'::ext_orc15;
SELECT '120'::ext_orc15 < '120'::ext_orc15;
SELECT '120'::ext_orc15 <= '120'::ext_orc15;
SELECT '120'::ext_orc15 <> '120'::ext_orc15;
SELECT '120'::ext_orc15 = '120'::ext_orc15;

-- comparison negation
SELECT NOT '120'::ext_orc15 > '120'::ext_orc15;
SELECT NOT '120'::ext_orc15 >= '120'::ext_orc15;
SELECT NOT '120'::ext_orc15 < '120'::ext_orc15;
SELECT NOT '120'::ext_orc15 <= '120'::ext_orc15;
SELECT NOT '120'::ext_orc15 <> '120'::ext_orc15;
SELECT NOT '120'::ext_orc15 = '120'::ext_orc15;

--commutator and negator
BEGIN;
CREATE TABLE ext_orc15_test AS
SELECT i::ext_orc15 as val FROM generate_series(1,10000) i;
CREATE INDEX ON ext_orc15_test(val);
ANALYZE;
SET enable_seqscan TO off;
EXPLAIN (COSTS OFF) SELECT * FROM ext_orc15_test where NOT val < 'c1';
EXPLAIN (COSTS OFF) SELECT * FROM ext_orc15_test where NOT 'c1' > val;
EXPLAIN (COSTS OFF) SELECT * FROM ext_orc15_test where 'c1' > val;
-- hash aggregate
SET enable_seqscan TO on;
EXPLAIN (COSTS OFF) SELECT val, COUNT(*) FROM ext_orc15_test GROUP BY 1;
ROLLBACK;