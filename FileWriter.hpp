/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2024, Sauron
 ******************************************************************************/

#ifndef __ROHAN_FILEWRITER_HPP
#define __ROHAN_FILEWRITER_HPP

#include <unix++/File.hpp>
#include "Writer.hpp"

namespace rohan {

/**/
class FileWriter : public Writer {
public:
    /** Open a file for writing **/
    explicit FileWriter(const char * filename, int flags=O_WRONLY|O_CREAT|O_TRUNC);
    /** Write a portion of data **/
    void write(const void * from, size_t length) override;
    /** Direct access to the underlying file **/
    upp::File &getFile() { return file; }
    
private:
    upp::File file;
};

}

#endif
