

GIT_VERSION := $(shell git --no-pager describe --tags --always)
GIT_COMMIT  := $(shell git rev-parse --verify HEAD)
GIT_DATE    := "$(shell git --no-pager show --date=iso --format="%ad" --name-only | head -n 1)"
BUILD_DATE  := "$(shell date --rfc-3339=seconds)"

# If working tree is dirty, append dirty flag
ifneq ($(strip $(shell git status --porcelain 2>/dev/null)),)
 GIT_VERSION := $(GIT_VERSION)-dirt
endif


CC=g++

#CFLAGS= -ggdb -w -Wall -fPIC -std=c++11
#CFLAGS= -g -O3 -w -Wall -fPIC -std=c++17
CFLAGS= -g -O0 -w -Wall -fPIC -std=c++17  \
        -DGIT_VERSION=\"$(GIT_VERSION)\"  \
        -DGIT_COMMIT=\"$(GIT_COMMIT)\"    \
        -DGIT_DATE=\"$(GIT_DATE)\"        \
        -DBUILD_DATE=\"$(BUILD_DATE)\"
LDFLAGS=../../3rdParty/base64/lib/libbase64.o
#LDLIBS=../HElib/src/fhe.a ../ntl-10.3.0/src/ntl.a -lgmp -lpthread
LDLIBS=-lstdc++fs  -lpthread -luuid
INCLUDE_PATH=   -I../inc                                \
                -I../../Util/inc                        \
                -I../../MemStream/inc                   \
                -I../../Logger/inc                      \
                -I../../SockLib/inc                     \
                -I../../3rdParty/rapidjson.1/include    \
                -I../../3rdParty/crc32                  \
                -I../../3rdParty/base64/include         \
                

SOURCES= $(wildcard ./*.cpp)
SOURCES+=$(wildcard ../src/*.cpp)
SOURCES+=$(wildcard ../../Util/src/*.cpp)
SOURCES+=$(wildcard ../../SockLib/src/*.cpp)
SOURCES+=$(wildcard ../../MemStream/src/*.cpp)
SOURCES+=$(wildcard ../../Logger/src/*.cpp)
SOURCES+=$(wildcard ../../3rdParty/crc32/*.cpp)

OBJECTS=$(SOURCES:.cpp=.o)
DEPENDS=$(SOURCES:.cpp=.d)

EXEC=StreamServer

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

%.o:%.cpp
	$(CC) -c $(INCLUDE_PATH) $(CFLAGS) $< -o $@

%.d:%.cpp
	$(CC) -c $(INCLUDE_PATH) $(CFLAGS) -M $< > $@

-include $(DEPENDS)

clean:
	rm -f $(OBJECTS) $(EXEC) $(DEPENDS)
