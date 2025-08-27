all: market-data-processor

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./src

SRC = src/main.cpp \
      src/data_processor/processor.cpp \
      src/file_handler/reader.cpp \
      src/file_handler/writer.cpp \
      src/models/market_data.cpp \
      src/utils/memory_monitor.cpp \
      src/utils/thread_pool.cpp

OBJ = $(SRC:.cpp=.o)

market-data-processor: $(OBJ)
	$(CXX) -o $@ $^ -lstdc++fs

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) market-data-processor

.PHONY: all clean