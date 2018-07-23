
# Compiler & linker.
CC=g++

# Compile flags.
#CFLAGS= -ggdb -w -Wall -fPIC -std=c++11
CFLAGS= -O3 -w -Wall -fPIC -std=c++17

# Link flags.
LDFLAGS=

# Linked libraries.
#LDLIBS=../HElib/src/fhe.a ../ntl-10.3.0/src/ntl.a -lgmp -lpthread
LDLIBS=

# Non-stand include paths.
INCLUDE_PATH=-I.. -I../ntl-10.3.0/include -I./inc

# All source files
SOURCES=$(wildcard *.cpp)
SOURCES+=$(wildcard src/*.cpp)

# Subsitute every '.cpp' prefix in $(SOURCES) variable with the '.o'.
OBJECTS=$(SOURCES:.cpp=.o)

# Subsitute every '.cpp' prefix in $(SOURCES) variable with the '.d'.
DEPENDS=$(SOURCES:.cpp=.d)

# The final executable file
EXEC=GenTestData

# Build final executable file
all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)


%.o:%.cpp
	$(CC) -c $(INCLUDE_PATH) $(CFLAGS) $< -o $@


%.d:%.cpp
	$(CC) -c $(INCLUDE_PATH) $(CFLAGS) -M $< > $@

-include $(DEPENDS)


# Remove executable file and all object files
clean:
	rm -f $(OBJECTS) $(EXEC) $(DEPENDS)


.PHONY: print-%
print-%: ; @echo $* = $($*)