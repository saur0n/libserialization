/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2024, Sauron
 ******************************************************************************/

#ifndef __ROHAN_FILEREADER_HPP
#define __ROHAN_FILEREADER_HPP

#include <unix++/File.hpp>
#include "Reader.hpp"

namespace rohan {

/**/
class FileReader : public Reader {
public:
    /** Open a file for reading **/
    explicit FileReader(const char * filename, int flags=O_RDONLY);
    /** Read a portion of data **/
    size_t read(void * to, size_t length) override;
    /** Skip a portion of data **/
    size_t skip(size_t length) override;
    /** Direct access to the underlying file **/
    upp::File &getFile() { return file; }
    
private:
    upp::File file;
};

}

#endif
