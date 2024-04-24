# compilation flags

CC=g++
CXXFLAGS += -std=c++20 -pedantic -Wall -Wextra -g -O0  -Iinclude -flto

# main

SRCDIR := src
BUILDDIR := build
TARGETDIR := bin

EXECUTABLE := main
TARGET := $(TARGETDIR)/$(EXECUTABLE)

SOURCES := $(filter-out $(wildcard $(SRCDIR)/*test.cpp),$(wildcard $(SRCDIR)/*.cpp))
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(TARGETDIR)
	$(CC) $(CXXFLAGS) -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CC) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(TARGETDIR):
	mkdir -p $(TARGETDIR)


# tests

TESTSOURCES := $(wildcard $(SRCDIR)/*.test.cpp)
TESTOBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(TESTSOURCES))
TESTEXECUTABLE := test_main
TESTTARGET := $(TARGETDIR)/$(TESTEXECUTABLE)

test: $(TESTOBJECTS) $(filter-out $(BUILDDIR)/main.o,$(OBJECTS))
	$(CC) $(CXXFLAGS) -o $(TESTTARGET) $^

$(BUILDDIR)/%.o: $(TESTDIR)/%.cpp | $(BUILDDIR)
	$(CC) $(CXXFLAGS) -c $< -o $@

# run

run: $(TARGET)
	./$(TARGET) OUCHLMM2.incoming.packets

run_test: test
	./$(TESTTARGET)

# clean

clean:
	rm -rf $(BUILDDIR) $(TARGETDIR)

.PHONY: all clean run test
