/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2019, Sauron
 ******************************************************************************/

#include <cstring>
#include "ByteArrayStream.hpp"

using namespace rohan;
using std::vector;

/******************************************************************************/

ByteArrayInputStream::ByteArrayInputStream(const vector<uint8_t> &buffer,
        size_t offset) : buffer(buffer), offset(offset) {}

void ByteArrayInputStream::read(void * to, size_t length) {
    if (buffer.size()<offset+length)
        throw End();
    memcpy(to, &buffer[offset], length);
    offset+=length;
}

/******************************************************************************/

ByteArrayOutputStream::ByteArrayOutputStream(vector<uint8_t> &buffer) :
        buffer(buffer) {}

void ByteArrayOutputStream::write(const void * from, size_t length) {
    size_t oldSize=buffer.size();
    buffer.resize(oldSize+length);
    memcpy(&buffer[oldSize], from, length);
}
