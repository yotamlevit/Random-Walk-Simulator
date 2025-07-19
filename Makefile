# Detect OS
UNAME_S := $(shell uname -s)
ifeq ($(OS),Windows_NT)
    IS_WINDOWS := 1
else
    IS_WINDOWS := 0
endif

# Default settings (Linux/g++)
CXX       := g++
CXXFLAGS  := -std=c++17 -fopenmp -O3 -Wall
LDFLAGS   :=
INCLUDES  :=
TARGET    := random_walk_simulator
SRC       := random_walk.cpp

# Adjust for macOS (need Homebrew's clang++)
ifeq ($(UNAME_S),Darwin)
    CXX := /opt/homebrew/opt/llvm/bin/clang++
    CXXFLAGS := -std=c++17 -fopenmp -O3 -Wall -I/opt/homebrew/opt/llvm/include
    LDFLAGS := -L/opt/homebrew/opt/llvm/lib
endif

# Adjust for Windows (MSYS2/MinGW)
ifeq ($(IS_WINDOWS),1)
    CXX := g++
    CXXFLAGS := -std=c++17 -fopenmp -O3 -Wall
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o data/*.csv

