################################################################################
#   Rohan data serialization library.
#
#   © 2016—2023, Sauron
################################################################################

CC=$(if $(TOOLCHAIN), /usr/bin/$(TOOLCHAIN)-)g++
CXXFLAGS=-Wall -Wextra -O3
LIBRARY=libserialization.so
HEADERS=*Serialization.hpp
SOURCES=*Serialization.cpp
UNITTEST=unittest

all: $(LIBRARY) $(UNITTEST)

clean:
	rm -f $(LIBRARY) $(UNITTEST) temporary.data

install: $(LIBRARY)
	install --strip $(LIBRARY) /usr/local/lib64
	cp -f *.hpp /usr/include/rohan

test: $(UNITTEST)
	./$(UNITTEST)

$(LIBRARY): $(SOURCES) $(HEADERS)
	$(CC) $(CXXFLAGS) -shared -fPIC -o $(LIBRARY) $(SOURCES) -lstdc++ -lunix++

$(UNITTEST): *.cpp *.hpp
	$(CC) $(CXXFLAGS) -o $(UNITTEST) *.cpp -lstdc++ -lunix++

.PHONY: all clean install test

