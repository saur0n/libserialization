################################################################################
#   Rohan data serialization library.
#
#   © 2016—2018, Sauron
################################################################################

CXXFLAGS=-Wall -std=gnu++11 -O3
LIBRARY=libserialization.so
UNITTEST=unittest

all: $(LIBRARY) $(UNITTEST)

clean:
	rm -f $(LIBRARY) $(UNITTEST) temporary.data

install:

test: $(UNITTEST)
	./$(UNITTEST)

$(LIBRARY): *.cpp *.hpp
	g++ $(CXXFLAGS) -shared -fPIC -o $(LIBRARY) -l stdc++ Stream.cpp

$(UNITTEST): *.cpp *.hpp
	g++ $(CXXFLAGS) -o $(UNITTEST) -l stdc++ *.cpp

.PHONY: all clean install test
