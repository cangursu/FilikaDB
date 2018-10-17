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
OBJS         += $(patsubst %.cpp,%.o,$(wildcard ../Util/src/*.cpp))
OBJS         += $(patsubst %.cpp,%.o,$(wildcard ../Logger/src/*.cpp))
OBJS         += $(patsubst %.cpp,%.o,$(wildcard ../MemStream/src/*.cpp))
OBJS         += $(patsubst %.cpp,%.o,$(wildcard ../MemStreamServer/src/*.cpp))
OBJS         += $(patsubst %.cpp,%.o,$(wildcard ../SockLib/src/*.cpp))
OBJS         += $(patsubst %.cpp,%.o,$(wildcard ../3rdparty/crc32/*.cpp))


# final shared library to be build from multiple source files (OBJS)
MODULE_big    = $(EXTENSION)


# Additional  directories
LIB_DIR_ORC   = /home/postgres/projects/orc/build/_CPack_Packages/Linux/TGZ/ORC-1.5.2-Linux/lib
INC_DIR_ORC   = /home/postgres/projects/orc/build/_CPack_Packages/Linux/TGZ/ORC-1.5.2-Linux/include
INC_DIR_LOG   = ../Logger/inc/
INC_DIR_UTL   = ../Util/inc/
INC_DIR_MEM   = ../MemStream/inc/
INC_DIR_SRV   = ../MemStreamServer/inc/
INC_DIR_SCK   = ../SockLib/inc/
INC_DIR_CRC   = ../3rdparty/crc32
INC_DIR_JSN   = ../3rdparty/rapidjson.1/include
INC_DIR_B64   = ../3rdparty/base64/include


INC_DIRS      = $(INC_DIR_ORC) $(INC_DIR_LOG) $(INC_DIR_MEM) $(INC_DIR_SRV) \
                $(INC_DIR_SCK) $(INC_DIR_CRC) $(INC_DIR_JSN) $(INC_DIR_B64) $(INC_DIR_UTL)
LIB_DIRS      = $(LIB_DIR_ORC)

CUSTOM_CC     = g++

PG_CPPFLAGS   = -fPIC -std=c++17 -rdynamic         \
                 $(foreach d, $(INC_DIRS), -I$d)   \



#PG_LIBS = -libstdc++
CUSTOM_COPT   = 



SHLIB_LINK    = -lstdc++ -rdynamic -shared         \
                $(foreach d, $(LIB_DIRS), -L$d)	   \
                -lorc                              \
                -lprotobuf                         \
                -lz                                \
                -lsnappy                           \
                -llz4                              \
                ../3rdparty/base64/lib/libbase64.o \




# postgres build stuff
PG_CONFIG = /usr/local/pgsql/bin/pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)


.PHONY: print-%
print-%: ; @echo $* = $($*)

