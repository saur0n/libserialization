/*******************************************************************************
 *  Rohan data serialization library.
 *  Byte array serialization
 *  
 *  © 2016—2023, Sauron
 ******************************************************************************/

#include <cstring>
#include "ByteArraySerialization.hpp"

using namespace rohan;
using std::vector;

/******************************************************************************/

ByteArrayReader::ByteArrayReader(const void * data, size_t length) :
        data(data), length(length), offset(0) {}

ByteArrayReader::ByteArrayReader(const vector<uint8_t> &buffer,
        size_t offset) : data(buffer.data()), length(buffer.size()), offset(offset) {}

size_t ByteArrayReader::read(void * to, size_t length) {
    if (available()<length)
        length-=available();
    memcpy(to, reinterpret_cast<const uint8_t *>(data)+offset, length);
    offset+=length;
    return length;
}

size_t ByteArrayReader::available() const {
    return length-offset;
}

/******************************************************************************/

static void appendTo(vector<uint8_t> &buffer, const void * from, size_t length) {
    size_t oldSize=buffer.size();
    buffer.resize(oldSize+length);
    memcpy(&buffer[oldSize], from, length);
}

ByteArrayWriter::ByteArrayWriter(size_t capacity) {
    buffer.reserve(capacity);
}

void ByteArrayWriter::write(const void * from, size_t length) {
    appendTo(buffer, from, length);
}

/*******************************************************************************/

ByteArrayRefWriter::ByteArrayRefWriter(vector<uint8_t> &buffer) :
        buffer(buffer) {}

void ByteArrayRefWriter::write(const void * from, size_t length) {
    appendTo(buffer, from, length);
}
