/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2023, Sauron
 ******************************************************************************/

#ifndef __ROHAN_FILESERIALIZATION_HPP
#define __ROHAN_FILESERIALIZATION_HPP

#include <unix++/File.hpp>
#include "Serialization.hpp"

namespace rohan {

/** Base class for FileReader and FileWriter **/
class FileSerializer {
protected:
    FileSerializer(const char * filename, int flags);
    upp::File file;
};

/**/
class FileReader : public Reader, FileSerializer {
public:
    /** Open a file for reading **/
    explicit FileReader(const char * filename, int flags=O_RDONLY);
    /** Read a portion of data **/
    size_t read(void * to, size_t length) override;
};

/**/
class FileWriter : public Writer, FileSerializer {
public:
    /** Open a file for writing **/
    explicit FileWriter(const char * filename, int flags=O_WRONLY|O_CREAT|O_TRUNC);
    /** Write a portion of data **/
    void write(const void * from, size_t length) override;
};

}

#endif
