################################################################################
#   Rohan data serialization library.
#
#   © 2016—2020, Sauron
################################################################################

CXXFLAGS=-Wall -Wextra -std=gnu++11 -O3
LIBRARY=libserialization.so
SOURCES=Serialization.hpp ByteArraySerialization.hpp FileSerialization.hpp
UNITTEST=unittest

all: $(LIBRARY) $(UNITTEST)

clean:
	rm -f $(LIBRARY) $(UNITTEST) temporary.data

install: $(LIBRARY)
	install --strip $(LIBRARY) /usr/local/lib64
	cp -f *.hpp /usr/include/rohan

test: $(UNITTEST)
	./$(UNITTEST)

$(LIBRARY): $(SOURCES) *.hpp
	g++ $(CXXFLAGS) -shared -fPIC -o $(LIBRARY) -l stdc++ $(SOURCES)

$(UNITTEST): *.cpp *.hpp
	g++ $(CXXFLAGS) -o $(UNITTEST) -l stdc++ *.cpp

.PHONY: all clean install test

