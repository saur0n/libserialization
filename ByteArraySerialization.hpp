/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2019, Sauron
 ******************************************************************************/

#ifndef __ROHAN_BYTEARRAYSTREAM_HPP
#define __ROHAN_BYTEARRAYSTREAM_HPP

#include "Serialization.hpp"

namespace rohan {

/** Reader for buffered data **/
class ByteArrayReader : public Reader {
public:
    /**/
    ByteArrayReader(const std::vector<uint8_t> &buffer, size_t offset=0);
    /** Returns underlying byte array **/
    const std::vector<uint8_t> &getBuffer() const { return buffer; }
    /**/
    void read(void * to, size_t length);
    
private:
    const std::vector<uint8_t> &buffer;
    size_t offset;
};

/** Writer for buffered data **/
class ByteArrayWriter : public Writer {
public:
    /**/
    ByteArrayWriter(std::vector<uint8_t> &buffer);
    /** Returns underlying byte array **/
    std::vector<uint8_t> &getBuffer() const { return buffer; }
    /**/
    void write(const void * from, size_t length);
    
private:
    std::vector<uint8_t> &buffer;
};

}

#endif
