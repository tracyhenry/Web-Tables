CXX=g++
CXXFLAGS=-O3 
TARGET=main

SRC=$(shell ls *.cpp)

all: $(TARGET)

$(TARGET): $(SRC:.cpp=.o)
	$(CXX) $(CXXFLAGS) $+ -o $@

-include $(SRC:.cpp=.d)

clean:
	$(RM) *.o *.d $(TARGET)

.PHONY: all clean

