/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2023, Sauron
 ******************************************************************************/

#include "FileSerialization.hpp"

using namespace rohan;

/******************************************************************************/

FileSerializer::FileSerializer(const char * filename, int flags) :
        file(filename, flags) {}

/******************************************************************************/

FileReader::FileReader(const char * filename, int flags) :
        FileSerializer(filename, flags) {}

size_t FileReader::read(void * to, size_t length) {
    return file.read(to, length);
}

/******************************************************************************/

FileWriter::FileWriter(const char * filename, int flags) :
        FileSerializer(filename, flags) {}

void FileWriter::write(const void * from, size_t length) {
    file.write(from, length);
}
