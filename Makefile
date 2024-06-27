################################################################################
#   Rohan data serialization library.
#
#   © 2016—2024, Sauron
################################################################################

CC=$(if $(TOOLCHAIN), /usr/bin/$(TOOLCHAIN)-)g++
CXXFLAGS=-Wall -Wextra -O3
LIBRARY=libserialization.so
HEADERS=*.hpp
SOURCES=*.cpp
LIBRARIES=-lstdc++ -lunix++
UNITTEST=unittest

all: $(LIBRARY) $(UNITTEST)

clean:
	rm -f $(LIBRARY) $(UNITTEST) temporary.data

install: $(LIBRARY)
	install --strip $(LIBRARY) /usr/local/lib64
	install -d /usr/include/rohan
	install -m 644 *.hpp /usr/include/rohan

test: $(UNITTEST)
	./$(UNITTEST)

$(LIBRARY): $(SOURCES) $(HEADERS)
	$(CC) $(CXXFLAGS) -shared -fPIC -o $(LIBRARY) $(SOURCES) $(LIBRARIES)

$(UNITTEST): $(SOURCES) $(HEADERS) ut/*
	$(CC) $(CXXFLAGS) -o $(UNITTEST) $(SOURCES) ut/* $(LIBRARIES)

.PHONY: all clean install test

