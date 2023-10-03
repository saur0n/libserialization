/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2023, Sauron
 ******************************************************************************/

#include "FileWriter.hpp"

using namespace rohan;

/******************************************************************************/

FileWriter::FileWriter(const char * filename, int flags) :
        file(filename, flags) {}

void FileWriter::write(const void * from, size_t length) {
    file.write(from, length);
}
