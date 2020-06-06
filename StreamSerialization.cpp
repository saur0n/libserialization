/*******************************************************************************
 *  Rohan data serialization library.
 *  Serialization to unix++ streams
 * 
 *  © 2020, Sauron
 ******************************************************************************/

#include "StreamSerialization.hpp"

using namespace rohan;
using nx::Stream;

StreamSerializer::StreamSerializer(nx::Stream &stream) : stream(stream) {}

/******************************************************************************/

StreamReader::StreamReader(nx::Stream &stream) : StreamSerializer(stream) {}

void StreamReader::read(void * to, size_t length) {
    size_t n=stream.read(to, length);
    if (n!=length)
        throw End();
}

/******************************************************************************/

StreamWriter::StreamWriter(nx::Stream &stream) : StreamSerializer(stream) {}

void StreamWriter::write(const void * from, size_t length) {
    size_t n=stream.write(from, length);
    if (n!=length)
        throw End();
}
