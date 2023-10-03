/*******************************************************************************
 *  Rohan data serialization library
 *  Unit tests
 *  
 *  © 2016—2023, Sauron
 ******************************************************************************/

#include <cassert>
#include <cstring>
#include <iostream>
#include "../BufferedReader.hpp"
#include "../FileReader.hpp"
#include "../FileSerialization.hpp"

using namespace rohan;
using namespace std;

const string EMPTY_STRING;
const string TEST_STRING="The quick brown fox jumps over the lazy dog";

class Record {
public:
    Record(unsigned id, const char * str) : id(id), str(str) {}
    Record(Reader &reader) : id(unsigned(reader)), str(string(reader)) {}
    void serialize(Writer &writer) const {
        writer | id | str;
    }
    
    unsigned id;
    string str;
};

template <class T>
map<string, T> createMap(std::initializer_list<T> list) {
    map<string, T> result;
    for (auto i=list.begin(); i!=list.end(); ++i)
        result[std::to_string(i-list.begin())]=*i;
    return result;
}

template <class T>
void testContainers(Writer &writer, std::initializer_list<T> list) {
    using Array=array<T, 16>;
    using Pair=pair<string, T>;
    using Map=map<string, T>;
    
    // Fixed arrays
    Array array;
    T carray[16];
    for (size_t i=0; i<16; i++)
        array[i]=carray[i]=list.begin()[i%list.size()];
    writer | carray | array;
    
    // Vector
    writer | vector<T>() | vector<T>(list);
    
    // Set
    writer | set<T>() | set<T>(list);
    
    // Pair
    writer | Pair("key", *list.begin());
    
    // Map
    writer | Map() | createMap(list);
}

template <class T>
void testContainers(Reader &reader, std::initializer_list<T> list) {
    using Array=array<T, 16>;
    using Pair=pair<string, T>;
    using Map=map<string, T>;
    
    // Fixed arrays
    Array expected;
    for (size_t i=0; i<16; i++)
        expected[i]=list.begin()[i%list.size()];
    assert(expected==Array(reader)); // C++ array
    assert(expected==Array(reader)); // C array
    
    // Vector
    assert(vector<T>(reader)==vector<T>());
    assert(vector<T>(reader)==vector<T>(list));
    
    // Set
    assert(set<T>(reader)==set<T>());
    assert(set<T>(reader)==set<T>(list));
    
    // Pair
    assert(Pair(reader)==Pair("key", *list.begin()));
    
    // Map
    assert(Map(reader)==Map());
    assert(Map(reader)==createMap(list));
}

void testWriter(Writer &writer) {
    // Basic types
    writer | false | true;
    writer | uint8_t(100) | uint8_t(200);
    writer | int8_t(-100) | int8_t(100);
    writer | uint16_t(10050) | uint16_t(20050);
    writer | int16_t(-10050) | int16_t(10050);
    writer | uint32_t(100500) | uint32_t(200500);
    writer | int32_t(-100500) | int32_t(100500);
    writer | uint64_t(100500100500ULL) | uint64_t(200300200300ULL);
    writer | int64_t(-100500100500LL) | int64_t(100500100500LL);
    
    // String types
    writer | "" | "The moose in the noose ate the goose who was loose";
    writer | EMPTY_STRING | TEST_STRING;
    
    // Serializables
    writer | Record(324930, "alpha") | Record(492042, "beta");
    
    // Linear containers
    testContainers<unsigned>(writer, {1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144});
    testContainers<string>(writer, {"alpha", "beta", "gamma", "delta"});
}

void testReader(Reader &reader) {
    // Basic types
    assert(bool(reader)==false);
    assert(bool(reader)==true);
    assert(uint8_t(reader)==100);
    assert(uint8_t(reader)==200);
    assert(int8_t(reader)==-100);
    assert(int8_t(reader)==100);
    assert(uint16_t(reader)==10050);
    assert(uint16_t(reader)==20050);
    assert(int16_t(reader)==-10050);
    assert(int16_t(reader)==10050);
    assert(uint32_t(reader)==100500);
    assert(uint32_t(reader)==200500);
    assert(int32_t(reader)==-100500);
    assert(int32_t(reader)==100500);
    assert(uint64_t(reader)==100500100500ULL);
    assert(uint64_t(reader)==200300200300ULL);
    assert(int64_t(reader)==-100500100500LL);
    assert(int64_t(reader)==100500100500LL);
    
    // String types
    assert(string(reader).empty());
    assert(string(reader)=="The moose in the noose ate the goose who was loose");
    assert(string(reader).empty());
    assert(string(reader)==TEST_STRING);
    
    // Serializables
    Record a(reader);
    Record b=Record(reader);
    
    // Linear containers
    testContainers<unsigned>(reader, {1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144});
    testContainers<string>(reader, {"alpha", "beta", "gamma", "delta"});
}

void testBufferedReader() {
    // Generate a test string
    vector<uint8_t> rstring(1000000u);
    for (size_t i=0; i<rstring.size(); i++)
        rstring[i]=i;//rand();
    
    // Write the string into a file
    upp::File fout("/tmp/serialization.test", O_CREAT|O_TRUNC|O_WRONLY);
    fout.write(rstring.data(), rstring.size());
    
    // Use buffered input stream for reading the file
    FileReader fr("/tmp/serialization.test");
    BufferedReader br(fr, 128);
    size_t offset=0, nRead=0;
    do {
        uint8_t temp[256];
        size_t fragmentLength=1+(rand()%256);
        nRead=br.read(temp, fragmentLength);
        assert(0==memcmp(&rstring[offset], temp, nRead));
        offset+=nRead;
    } while (nRead);
    
    assert(offset==rstring.size());
}

int main(int argc, char ** argv) {
    (void)argc;
    (void)argv;
    
    FileWriter fw("/tmp/serialization.test");
    testWriter(fw);
    
    FileReader fr("/tmp/serialization.test");
    testReader(fr);
    
    testBufferedReader();
    
    cout << "SUCCESS!" << endl;
    return 0;
}
