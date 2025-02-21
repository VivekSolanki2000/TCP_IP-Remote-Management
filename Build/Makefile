CXX = g++
CXXFLAGS = -Wall -std=c++17
LDFLAGS = -pthread

# Directories
SRCDIR = ../Source
BUILDDIR = .
TARGET = tcpapp

# Source files and object files
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

# Main target
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Pattern rule for object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(BUILDDIR)/*.o $(TARGET)

.PHONY: clean