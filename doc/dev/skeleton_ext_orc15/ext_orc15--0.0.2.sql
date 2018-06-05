-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION ext_orc15" to load this file. \quit

CREATE FUNCTION ext_orc15_in(cstring)
RETURNS ext_orc15
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION ext_orc15_out(ext_orc15)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE ext_orc15 (
  INPUT          = ext_orc15_in,
  OUTPUT         = ext_orc15_out,
  LIKE           = integer
);

CREATE FUNCTION ext_orc15_eq(ext_orc15, ext_orc15)
RETURNS boolean LANGUAGE internal IMMUTABLE AS 'int4eq';

CREATE FUNCTION ext_orc15_ne(ext_orc15, ext_orc15)
RETURNS boolean LANGUAGE internal IMMUTABLE AS 'int4ne';

CREATE FUNCTION ext_orc15_lt(ext_orc15, ext_orc15)
RETURNS boolean LANGUAGE internal IMMUTABLE AS 'int4lt';

CREATE FUNCTION ext_orc15_le(ext_orc15, ext_orc15)
RETURNS boolean LANGUAGE internal IMMUTABLE AS 'int4le';

CREATE FUNCTION ext_orc15_gt(ext_orc15, ext_orc15)
RETURNS boolean LANGUAGE internal IMMUTABLE AS 'int4gt';

CREATE FUNCTION ext_orc15_ge(ext_orc15, ext_orc15)
RETURNS boolean LANGUAGE internal IMMUTABLE AS 'int4ge';

CREATE FUNCTION ext_orc15_cmp(ext_orc15, ext_orc15)
RETURNS integer LANGUAGE internal IMMUTABLE AS 'btint4cmp';

CREATE FUNCTION hash_ext_orc15(ext_orc15)
RETURNS integer LANGUAGE internal IMMUTABLE AS 'hashint4';

CREATE OPERATOR = (
  LEFTARG = ext_orc15,
  RIGHTARG = ext_orc15,
  PROCEDURE = ext_orc15_eq,
  COMMUTATOR = '=',
  NEGATOR = '<>',
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  HASHES, MERGES
);

CREATE OPERATOR <> (
  LEFTARG = ext_orc15,
  RIGHTARG = ext_orc15,
  PROCEDURE = ext_orc15_ne,
  COMMUTATOR = '<>',
  NEGATOR = '=',
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

CREATE OPERATOR < (
  LEFTARG = ext_orc15,
  RIGHTARG = ext_orc15,
  PROCEDURE = ext_orc15_lt,
  COMMUTATOR = > ,
  NEGATOR = >= ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE OPERATOR <= (
  LEFTARG = ext_orc15,
  RIGHTARG = ext_orc15,
  PROCEDURE = ext_orc15_le,
  COMMUTATOR = >= ,
  NEGATOR = > ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

CREATE OPERATOR > (
  LEFTARG = ext_orc15,
  RIGHTARG = ext_orc15,
  PROCEDURE = ext_orc15_gt,
  COMMUTATOR = < ,
  NEGATOR = <= ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE OPERATOR >= (
  LEFTARG = ext_orc15,
  RIGHTARG = ext_orc15,
  PROCEDURE = ext_orc15_ge,
  COMMUTATOR = <= ,
  NEGATOR = < ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

CREATE OPERATOR CLASS btree_ext_orc15_ops
DEFAULT FOR TYPE ext_orc15 USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       ext_orc15_cmp(ext_orc15, ext_orc15);

CREATE OPERATOR CLASS hash_ext_orc15_ops
    DEFAULT FOR TYPE ext_orc15 USING hash AS
        OPERATOR        1       = ,
        FUNCTION        1       hash_ext_orc15(ext_orc15);

CREATE CAST (integer as ext_orc15) WITHOUT FUNCTION AS ASSIGNMENT;
CREATE CAST (ext_orc15 as integer) WITHOUT FUNCTION AS ASSIGNMENT;

-- big_ext_orc15

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
