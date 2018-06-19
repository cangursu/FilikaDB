# the extensions name
EXTENSION     = orcinmem
DATA          = $(wildcard *--*.sql)        # script files to install
TESTS         = $(wildcard test/sql/*.sql)  # use test/sql/*.sql as testfiles

# find the sql and expected directories under test
# load plpgsql into test db
# load base36 extension into test db
# dbname
REGRESS_OPTS  = --inputdir=test               \
                --load-extension=$(EXTENSION) \
                --load-language=plpgsql
REGRESS       = $(patsubst test/sql/%.sql,%,$(TESTS))
OBJS          = $(patsubst %.c,%.o,$(wildcard src/*.c))
OBJS         += $(patsubst %.cpp,%.o,$(wildcard src/*.cpp))
OBJS         += $(patsubst %.cpp,%.o,$(wildcard Logger/src/*.cpp))

# final shared library to be build from multiple source files (OBJS)
MODULE_big    = $(EXTENSION)


# Additional  directories
LIB_DIR_ORC   = /home/postgres/projects/orc/build/_CPack_Packages/Linux/TGZ/ORC-1.5.1-SNAPSHOT-Linux/lib
INC_DIR_ORC   = /home/postgres/projects/orc/build/_CPack_Packages/Linux/TGZ/ORC-1.5.1-SNAPSHOT-Linux/include
INC_DIR_LOG   = Logger/inc/

INC_DIRS      = $(INC_DIR_ORC) $(INC_DIR_LOG)
LIB_DIRS      = $(LIB_DIR_ORC)


PG_CPPFLAGS = -fPIC -std=c++11 -rdynamic      \
              $(foreach d, $(INC_DIRS), -I$d) \

              

#PG_LIBS = -libstdc++
CUSTOM_COPT = -lstdc++ -rdynamic -shared      \
              $(foreach d, $(LIB_DIRS), -L$d) \
              -lorc                           \
              -lprotobuf                      \
              -lz                             \
              -lsnappy                        \
              -llz4                           \





# postgres build stuff
PG_CONFIG = /usr/local/pgsql/bin/pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)


.PHONY: print-%
print-%: ; @echo $* = $($*)
