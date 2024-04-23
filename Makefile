CC=g++
CXXFLAGS += -std=c++20 -pedantic -Wall -Wextra -g -O0 

# Directories
SRCDIR := src
BUILDDIR := build
TARGETDIR := bin

# Targets
EXECUTABLE := main
TARGET := $(TARGETDIR)/$(EXECUTABLE)

# Code lists
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

# Default make
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJECTS)
	$(CC) $(CXXFLAGS) -o $@ $^

# Compile the source files into object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CC) $(CXXFLAGS) -c $< -o $@

# Link the target executable
$(TARGET): $(OBJECTS) | $(TARGETDIR)
	$(CC) $(CXXFLAGS) -o $@ $^

# Ensure the build and target directories exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(TARGETDIR):
	mkdir -p $(TARGETDIR)

# ... [rest of your Makefile] ...

# Phony targets
.PHONY: all clean run

run: $(TARGET)
	./$(TARGET) OUCHLMM2.incoming.packets
