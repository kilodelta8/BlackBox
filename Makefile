CXX = g++
CXXFLAGS = -Wall -O2 -pthread
SRC = firmware/witty_manager.cpp
TARGET = bin/witty_manager

all:
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -rf bin
	rm -f /tmp/bb_tx.sock /tmp/bb_rx.sock

run: all
	sudo ./$(TARGET)
