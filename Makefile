CXX = g++
CXXFLAGS = -Wall -pthread
TARGET = tcpapp
SRCS = Main.cpp NetworkSettings.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(CXXFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean