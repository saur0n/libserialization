/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2024, Sauron
 ******************************************************************************/

#include "FileReader.hpp"

using namespace rohan;

/******************************************************************************/

FileReader::FileReader(const char * filename, int flags) :
        file(filename, flags) {}

size_t FileReader::read(void * to, size_t length) {
    return file.read(to, length);
}

size_t FileReader::skip(size_t length) {
    return file.seek(length, SEEK_CUR);
}
