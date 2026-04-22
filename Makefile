CXX ?= c++
CPPFLAGS ?= -Iheaders
CXXFLAGS ?= -O3 -std=c++17

TARGET := raytracer
SRC := main.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) result.ppm
