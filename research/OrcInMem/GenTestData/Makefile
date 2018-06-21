
# Compiler & linker.
CC=g++

# Compile flags.
#CFLAGS= -ggdb -w -Wall -fPIC -std=c++11
CFLAGS= -O3 -w -Wall -fPIC -std=c++11

# Link flags.
LDFLAGS=

# Linked libraries.
#LDLIBS=../HElib/src/fhe.a ../ntl-10.3.0/src/ntl.a -lgmp -lpthread
LDLIBS=

# Non-stand include paths.
INCLUDE_PATH=-I.. -I../ntl-10.3.0/include -I./inc

# All header files
HEADERS=$(wildcard *.h)
HEADERS+=$(wildcard inc/*.h)

# All source files
SOURCES=$(wildcard *.cpp)
SOURCES+=$(wildcard src/*.cpp)

# Subsitute every '.cpp' prefix in $(SOURCES) variable with the '.o'.
OBJECTS=$(SOURCES:.cpp=.o)

# The final executable file
EXEC=GenTestData

# Build final executable file
all: $(EXEC)
$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# For insurance: if any header file is changed, re-compile the whole project.
%.o:%.cpp $(HEADERS)
	$(CC) -c $(INCLUDE_PATH) $(CFLAGS) $< -o $@

# Remove executable file and all object files
clean:
	rm -f $(OBJECTS) $(EXEC)
