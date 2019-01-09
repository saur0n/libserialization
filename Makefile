################################################################################
#   Rohan data serialization library.
#
#   © 2016—2019, Sauron
################################################################################

CXXFLAGS=-Wall -Wextra -std=gnu++11 -O3
LIBRARY=libserialization.so
UNITTEST=unittest

all: $(LIBRARY) $(UNITTEST)

clean:
	rm -f $(LIBRARY) $(UNITTEST) temporary.data

install:
	install --strip libserialization.so /usr/local/lib64
	cp -f *.hpp /usr/include

test: $(UNITTEST)
	./$(UNITTEST)

$(LIBRARY): *.cpp *.hpp
	g++ $(CXXFLAGS) -shared -fPIC -o $(LIBRARY) -l stdc++ serialization.cpp

$(UNITTEST): *.cpp *.hpp
	g++ $(CXXFLAGS) -o $(UNITTEST) -l stdc++ *.cpp

.PHONY: all clean install test
