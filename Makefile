CXX = g++
CXXFLAGS = -std=c++11 -pthread -O2 -march=native -mtune=native -flto -Ofast
TARGET = main
SRC = main.cpp 

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
