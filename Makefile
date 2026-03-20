CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -I. -fsanitize=address -g  # Adjust for httplib and json paths if needed
LDFLAGS = -fsanitize=address  # Add -l flags if needed

SRCS = src/main.cpp src/interface.cpp src/Cart.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = build/shop

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
