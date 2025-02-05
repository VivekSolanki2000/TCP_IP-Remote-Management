CXX = g++
CXXFLAGS = -Wall -pthread -std=c++17

# Directories
SRCDIR = ../Source
BUILDDIR = .
TARGET = tcpapp

# Source files and object files
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

# Main target
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(CXXFLAGS)

# Pattern rule for object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Clean rule
clean:
	rm -f $(BUILDDIR)/*.o $(TARGET)

.PHONY: clean