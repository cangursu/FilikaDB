-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION OrcInMem" to load this file. \quit

CREATE FUNCTION orc_inmem_test(text, text) RETURNS text
AS '$libdir/orcinmem'
LANGUAGE C IMMUTABLE STRICT;
