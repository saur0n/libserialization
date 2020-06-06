/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2020, Sauron
 ******************************************************************************/

#include <cstring>
#include "ByteArraySerialization.hpp"

using namespace rohan;
using std::vector;

/******************************************************************************/

ByteArrayReader::ByteArrayReader(const vector<uint8_t> &buffer,
        size_t offset) : buffer(buffer), offset(offset) {}

void ByteArrayReader::read(void * to, size_t length) {
    if (buffer.size()<offset+length)
        throw End();
    memcpy(to, &buffer[offset], length);
    offset+=length;
}

/******************************************************************************/

ByteArrayWriter::ByteArrayWriter(vector<uint8_t> &buffer) :
        buffer(buffer) {}

void ByteArrayWriter::write(const void * from, size_t length) {
    size_t oldSize=buffer.size();
    buffer.resize(oldSize+length);
    memcpy(&buffer[oldSize], from, length);
}
