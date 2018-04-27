##############################################################################
#
# Flowlog Foreign Data Wrapper for PostgreSQL
# 
# Copyright (c) 2013 Andrew Dunstan
# 
# This software is released under the PostgreSQL Licence
# 
# Author: Andrew Dunstan <andrew@dunslane.net>
# 
# IDENTIFICATION
#        flowlog_fdw/Makefile
# 
##############################################################################


EXTENSION    = flowlog_fdw
EXTVERSION   = $(shell grep default_version $(EXTENSION).control | sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")

DATA         = $(filter-out $(wildcard sql/*--*.sql),$(wildcard sql/*.sql))
DOCS         = $(wildcard doc/*.md)
USE_MODULE_DB = 1
TESTS        = $(wildcard test/sql/*.sql)
REGRESS      = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test --outputdir=test \
	--load-language=plpgsql --load-extension=$(EXTENSION)
MODULE_big   = $(EXTENSION)
OBJS         = $(patsubst %.c,%.o,$(wildcard src/*.c))
OBJS        += $(patsubst %.cpp,%.o,$(wildcard src/*.cpp))
OBJS        += $(patsubst %.cpp,%.o,$(wildcard Logger/src/*.cpp))
PG_CONFIG    = /usr/local/pgsql/bin/pg_config
PG_CPPFLAGS  = -fPIC -std=c++11 -Iinc -ILogger/inc
CUSTOM_COPT  = -lstdc++

all: sql/$(EXTENSION)--$(EXTVERSION).sql

sql/$(EXTENSION)--$(EXTVERSION).sql: sql/$(EXTENSION).sql
	cp $< $@

DATA_built = sql/$(EXTENSION)--$(EXTVERSION).sql
DATA = $(filter-out sql/$(EXTENSION)--$(EXTVERSION).sql, $(wildcard sql/*--*.sql))
EXTRA_CLEAN = sql/$(EXTENSION)--$(EXTVERSION).sql

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

# we put all the tests in a test subdir, but pgxs expects us not to, darn it
override pg_regress_clean_files = test/results/ test/regression.diffs test/regression.out tmp_check/ log/
