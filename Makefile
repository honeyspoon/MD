CC=g++
CXXFLAGS += -std=c++17 -pedantic -Wall -Wextra -g -O0 

PROGRAM = main
SOURCES = main.cpp stats.cpp parser.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(PROGRAM)

run: $(PROGRAM)
	./$(PROGRAM) OUCHLMM2.incoming.packets

$(PROGRAM): $(OBJECTS)
	$(CC) $(CXXFLAGS) -o $@ $^ 

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(PROGRAM) $(OBJECTS)

.PHONY: all run clean
