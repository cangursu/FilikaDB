
CC=g++

#CFLAGS= -ggdb -w -Wall -fPIC -std=c++11
CFLAGS= -g -O0 -w -Wall -fPIC -std=c++17
LDFLAGS=../../3rdparty/base64/lib/libbase64.o 
LDFLAGS+=-lpthread
#LDLIBS=../HElib/src/fhe.a ../ntl-10.3.0/src/ntl.a -lgmp -lpthread
LDLIBS=

INCLUDE_PATH=   -I..                                \
                -I../inc                            \
                -I../../Util/inc                    \
                -I../../MemStream/inc               \
                -I../../MemStreamServer/inc         \
                -I../../SockLib/inc                 \
                -I../../3rdparty/crc32              \
                -I../../3rdparty/base64/include     \
                -I../../3rdparty/rapidjson.1/include\
                

SOURCES=$(wildcard *.cpp)
SOURCES+=$(wildcard ../src/*.cpp)
SOURCES+=$(wildcard ../../Util/src/*.cpp)
SOURCES+=$(wildcard ../../SockLib/src/*.cpp)
SOURCES+=$(wildcard ../../MemStream/src/*.cpp)
SOURCES+=$(wildcard ../../MemStreamServer/src/*.cpp)
SOURCES+=$(wildcard ../../3rdparty/crc32/*.cpp)

OBJECTS=$(SOURCES:.cpp=.o)
DEPENDS=$(SOURCES:.cpp=.d)

EXEC=LoggerServer

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
