CXX = g++
CXXFLAGS = -Wall -O2
SRC = firmware/witty_manager.cpp
TARGET = bin/witty_manager

all:
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -rf bin
	rm -f /tmp/blackbox.sock

run: all
	sudo ./$(TARGET)
