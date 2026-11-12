CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -static-libstdc++ -static-libgcc
TARGET = cstarc
SRC = cstcompiler.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
