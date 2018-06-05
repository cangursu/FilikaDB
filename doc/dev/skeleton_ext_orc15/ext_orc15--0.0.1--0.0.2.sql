-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION ext_orc15" to load this file. \quit

CREATE FUNCTION big_ext_orc15_in(cstring)
RETURNS big_ext_orc15
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION big_ext_orc15_out(big_ext_orc15)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE big_ext_orc15 (
  INPUT          = big_ext_orc15_in,
  OUTPUT         = big_ext_orc15_out,
  LIKE           = bigint
);

CREATE FUNCTION big_ext_orc15_eq(big_ext_orc15, big_ext_orc15)
RETURNS boolean LANGUAGE internal IMMUTABLE AS 'int8eq';

CREATE FUNCTION big_ext_orc15_ne(big_ext_orc15, big_ext_orc15)
RETURNS boolean LANGUAGE internal IMMUTABLE AS 'int8ne';

CREATE FUNCTION big_ext_orc15_lt(big_ext_orc15, big_ext_orc15)
RETURNS boolean LANGUAGE internal IMMUTABLE AS 'int8lt';

CREATE FUNCTION big_ext_orc15_le(big_ext_orc15, big_ext_orc15)
RETURNS boolean LANGUAGE internal IMMUTABLE AS 'int8le';

CREATE FUNCTION big_ext_orc15_gt(big_ext_orc15, big_ext_orc15)
RETURNS boolean LANGUAGE internal IMMUTABLE AS 'int8gt';

CREATE FUNCTION big_ext_orc15_ge(big_ext_orc15, big_ext_orc15)
RETURNS boolean LANGUAGE internal IMMUTABLE AS 'int8ge';

CREATE FUNCTION big_ext_orc15_cmp(big_ext_orc15, big_ext_orc15)
RETURNS integer LANGUAGE internal IMMUTABLE AS 'btint8cmp';

CREATE FUNCTION hash_big_ext_orc15(big_ext_orc15)
RETURNS integer LANGUAGE internal IMMUTABLE AS 'hashint8';

CREATE OPERATOR = (
  LEFTARG = big_ext_orc15,
  RIGHTARG = big_ext_orc15,
  PROCEDURE = big_ext_orc15_eq,
  COMMUTATOR = '=',
  NEGATOR = '<>',
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  HASHES, MERGES
);

CREATE OPERATOR <> (
  LEFTARG = big_ext_orc15,
  RIGHTARG = big_ext_orc15,
  PROCEDURE = big_ext_orc15_ne,
  COMMUTATOR = '<>',
  NEGATOR = '=',
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE OPERATOR < (
  LEFTARG = big_ext_orc15,
  RIGHTARG = big_ext_orc15,
  PROCEDURE = big_ext_orc15_lt,
  COMMUTATOR = > ,
  NEGATOR = >= ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE OPERATOR <= (
  LEFTARG = big_ext_orc15,
  RIGHTARG = big_ext_orc15,
  PROCEDURE = big_ext_orc15_le,
  COMMUTATOR = >= ,
  NEGATOR = > ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE OPERATOR > (
  LEFTARG = big_ext_orc15,
  RIGHTARG = big_ext_orc15,
  PROCEDURE = big_ext_orc15_gt,
  COMMUTATOR = < ,
  NEGATOR = <= ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE OPERATOR >= (
  LEFTARG = big_ext_orc15,
  RIGHTARG = big_ext_orc15,
  PROCEDURE = big_ext_orc15_ge,
  COMMUTATOR = '<=' ,
  NEGATOR = '<' ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE OPERATOR CLASS btree_big_ext_orc15_ops
DEFAULT FOR TYPE big_ext_orc15 USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       big_ext_orc15_cmp(big_ext_orc15, big_ext_orc15);

CREATE OPERATOR CLASS hash_big_ext_orc15_ops
DEFAULT FOR TYPE big_ext_orc15 USING hash AS
        OPERATOR        1       = ,
        FUNCTION        1       hash_big_ext_orc15(big_ext_orc15);

CREATE CAST (bigint as big_ext_orc15) WITHOUT FUNCTION AS ASSIGNMENT;
CREATE CAST (big_ext_orc15 as bigint) WITHOUT FUNCTION AS ASSIGNMENT;
