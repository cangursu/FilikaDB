
CC=g++

#CFLAGS= -ggdb -w -Wall -fPIC -std=c++11
CFLAGS= -g -O0 -w -Wall -fPIC -std=c++17
LDFLAGS=-lpthread -luuid
#LDLIBS=../HElib/src/fhe.a ../ntl-10.3.0/src/ntl.a -lgmp -lpthread
LDLIBS=../3rdParty/base64/lib/libbase64.o 
INCLUDE_PATH=-Iinc                                  \
             -I../Util/inc                       \
             -I../MemStream/inc                  \
             -I../MemStreamServer/inc            \
             -I../Logger/inc                     \
             -I../SockLib/inc                    \
             -I../3rdParty/crc32                 \
             -I../3rdParty/rapidjson.1/include   \
             -I../3rdParty/base64/include        \

SOURCES= $(wildcard *.cpp)
SOURCES+=$(wildcard src/*.cpp)
SOURCES+=$(wildcard ../Util/src/*.cpp)
SOURCES+=$(wildcard ../SockLib/src/*.cpp)
SOURCES+=$(wildcard ../Logger/src/*.cpp)
SOURCES+=$(wildcard ../MemStream/src/*.cpp)
SOURCES+=$(wildcard ../MemStreamServer/src/*.cpp)
SOURCES+=$(wildcard ../3rdParty/crc32/*.cpp)

OBJECTS=$(SOURCES:.cpp=.o)
DEPENDS=$(SOURCES:.cpp=.d)

EXEC=OrcRemoteServerTCP

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


.PHONY: print-%
print-%: ; @echo $* = $($*)
