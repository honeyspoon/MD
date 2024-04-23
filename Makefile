CC=g++

CXXFLAGS += -std=c++17 -pedantic -Wall -Wextra -g -O0 

PROGRAM = main
all: $(PROGRAM)

run: $(PROGRAM)
	./$(PROGRAM) OUCHLMM2.incoming.packets

$(PROGRAM): $(PROGRAM).cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ 

clean:
	rm -f $(PROGRAM)

.PHONY: clean