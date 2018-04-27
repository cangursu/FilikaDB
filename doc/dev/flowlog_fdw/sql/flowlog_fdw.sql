/*-------------------------------------------------------------------------
 *
 *                foreign-data wrapper  flowlog
 *
 * Copyright (c) 2013, PostgreSQL Global Development Group
 *
 * This software is released under the PostgreSQL Licence
 *
 * Author:  Andrew Dunstan <andrew@dunslane.net>
 *
 * IDENTIFICATION
 *                flowlog_fdw/=sql/flowlog_fdw.sql
 *
 *-------------------------------------------------------------------------
 */

CREATE FUNCTION flowlog_fdw_handler()
RETURNS fdw_handler
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION flowlog_fdw_validator(text[], oid)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FOREIGN DATA WRAPPER flowlog_fdw
  HANDLER flowlog_fdw_handler
  VALIDATOR flowlog_fdw_validator;
