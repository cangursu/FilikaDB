EXTENSION = base36        # the extensions name
DATA = base36--0.0.2.sql  # script files to install
REGRESS = base36_test     # our test script file (without extension)
MODULES = base36          # our c module file to build


# postgres build stuff
PG_CONFIG = /usr/local/pgsql/bin/pg_config
PG_CPPFLAGS = -fPIC -std=c++11
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
