/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2023, Sauron
 ******************************************************************************/

#include "FileReader.hpp"

using namespace rohan;

/******************************************************************************/

FileReader::FileReader(const char * filename, int flags) :
        file(filename, flags) {}

size_t FileReader::read(void * to, size_t length) {
    return file.read(to, length);
}
