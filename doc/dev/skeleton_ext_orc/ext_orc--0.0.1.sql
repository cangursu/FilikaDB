-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION ext_orc" to load this file. \quit

CREATE FUNCTION ext_orc_scan(text) RETURNS text
AS '$libdir/ext_orc'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION ext_orc_content(text) RETURNS text
AS '$libdir/ext_orc'
LANGUAGE C IMMUTABLE STRICT;
