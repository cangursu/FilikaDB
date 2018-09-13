# Declaration of variables
CC = g++
CC_FLAGS = -ggdb -w -Wall -fPIC -std=c++17  -I../inc

# File names
EXEC = LoggerSender
SOURCES = $(wildcard *.cpp)
SOURCES += $(wildcard ../src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(EXEC)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)

