/*******************************************************************************
 *  Rohan data serialization library.
 *  Serialization to unix++ streams
 * 
 *  © 2020—2024, Sauron
 ******************************************************************************/

#include "StreamSerialization.hpp"

using namespace rohan;
using upp::Stream;

StreamSerializer::StreamSerializer(Stream &stream) : stream(stream) {}

/******************************************************************************/

StreamReader::StreamReader(Stream &stream) : StreamSerializer(stream) {}

size_t StreamReader::read(void * to, size_t length) {
    return stream.read(to, length);
}

size_t StreamReader::skip(size_t length) {
    (void)length;
    throw "not implemented";
}

/******************************************************************************/

StreamWriter::StreamWriter(Stream &stream) : StreamSerializer(stream) {}

void StreamWriter::write(const void * from, size_t length) {
    size_t n=stream.write(from, length);
    if (n!=length)
        throw End();
}
