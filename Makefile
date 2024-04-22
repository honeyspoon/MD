CC=g++

CXXFLAGS += -std=c++17 -pedantic -Wall -Wextra -g -O0 -fprofile-instr-generate -fcoverage-mapping



PROGRAM = main
all: $(PROGRAM)

run: $(PROGRAM)
	./$(PROGRAM) OUCHLMM2.incoming.packets

$(PROGRAM): $(PROGRAM).cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ 

clean:
	rm -f $(PROGRAM) $(PROFILE_OUT) callgrind.out

