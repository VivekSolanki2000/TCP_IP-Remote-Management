CXX = g++
CXXFLAGS = -Wall -pthread -std=c++17
TARGET = tcpapp
SRCS = Main.cpp NetworkSettings.cpp History.cpp RemoteManagement.cpp MessageHandle.cpp ExecuteCommands.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(CXXFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
