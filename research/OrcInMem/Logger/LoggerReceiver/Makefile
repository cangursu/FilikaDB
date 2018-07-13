
CC=g++

#CFLAGS= -ggdb -w -Wall -fPIC -std=c++11
CFLAGS= -O3 -w -Wall -fPIC -std=c++17
LDFLAGS=
#LDLIBS=../HElib/src/fhe.a ../ntl-10.3.0/src/ntl.a -lgmp -lpthread
LDLIBS=
INCLUDE_PATH=-I.. -I../inc -I../../MemStream/inc -I../../MemStreamRemote/inc

SOURCES=$(wildcard *.cpp)
SOURCES+=$(wildcard ../src/*.cpp)
SOURCES+=$(wildcard ../../MemStream/src/*.cpp)
SOURCES+=$(wildcard ../../MemStreamRemote/src/*.cpp)

OBJECTS=$(SOURCES:.cpp=.o)
DEPENDS=$(SOURCES:.cpp=.d)

EXEC=LoggerReceiever

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
