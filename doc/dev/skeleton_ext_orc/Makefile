EXTENSION = ext_orc        # the extensions name
DATA = ext_orc--0.0.1.sql  # script files to install
REGRESS = ext_orc_test     # our test script file (without extension)
MODULES = ext_orc          # our c module file to build


# postgres build stuff

PG_CONFIG = /usr/local/pgsql/bin/pg_config

PG_CPPFLAGS = -fPIC -std=c++11 -rdynamic                                  \
              -I/home/postgres/projects/orc/c++/include                   \
              -I/home/postgres/projects/orc/c++/src                       \
              -I/home/postgres/projects/orc/build/c++/include             \
              -I/home/postgres/projects/orc/build/c++/src                 \
              -I/home/postgres/projects/orc/c++/libs/protobuf-2.6.0/src   \

#PG_LIBS = -libstdc++
CUSTOM_COPT = -lstdc++ -rdynamic -shared                                  \
              -L/home/postgres/projects/orc/build/c++/src                 \
              -L/home/postgres/projects/orc/build/c++/libs/protobuf-2.6.0 \
              -L/home/postgres/projects/orc/build/c++/libs/zlib-1.2.8     \
              -L/home/postgres/projects/orc/build/c++/libs/snappy-1.1.2   \
              -L/home/postgres/projects/orc/build/c++/libs/lz4-r131       \
              -lorc                                                       \
              -lprotobuf                                                  \
              -lz                                                         \
              -lsnappy                                                    \
              -llz4                                                       \

SHLIB_LINK =    -l/home/postgres/projects/orc/build/c++/src/liborc.a                      \
                -l/home/postgres/projects/orc/build/c++/libs/protobuf-2.6.0/libprotobuf.a \
                -l/home/postgres/projects/orc/build/c++/libs/zlib-1.2.8/libz.a            \
                -l/home/postgres/projects/orc/build/c++/libs/snappy-1.1.2/libsnappy.a     \
                -l/home/postgres/projects/orc/build/c++/libs/lz4-r131/liblz4.a 

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
