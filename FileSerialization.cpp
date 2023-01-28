#include "FileSerialization.hpp"

using namespace rohan;

/******************************************************************************/

FileSerializer::FileSerializer(const char * filename, int flags) :
        file(filename, flags) {}

/******************************************************************************/

FileReader::FileReader(const char * filename, int flags) :
        FileSerializer(filename, flags) {}

void FileReader::read(void * to, size_t length) {
    if (length!=file.read(to, length))
        throw End();
}

/******************************************************************************/

FileWriter::FileWriter(const char * filename, int flags) :
        FileSerializer(filename, flags) {}

void FileWriter::write(const void * from, size_t length) {
    file.write(from, length);
}
