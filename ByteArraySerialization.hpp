/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2021, Sauron
 ******************************************************************************/

#ifndef __ROHAN_BYTEARRAYSTREAM_HPP
#define __ROHAN_BYTEARRAYSTREAM_HPP

#include "Serialization.hpp"

namespace rohan {

/** Reader for buffered data **/
class ByteArrayReader : public Reader {
public:
    /** Initialize from pointer and length **/
    ByteArrayReader(const void * data, size_t length);
    /** Initialize from byte array **/
    ByteArrayReader(const std::vector<uint8_t> &buffer, size_t offset=0);
    /** Return pointer to the start of byte array **/
    const void * getData() const { return data; }
    /** Returns total data length **/
    size_t getLength() const { return length; }
    /** Read a portion of data **/
    void read(void * to, size_t length) override;
    /** Returns the number of bytes which are already read **/
    size_t consumed() const { return offset; }
    /** Returns the number of bytes that can be read **/
    size_t available() const;
    
private:
    const void * data;
    size_t length;
    size_t offset;
};

/** Writer for buffered data **/
class ByteArrayWriter : public Writer {
public:
    /**/
    explicit ByteArrayWriter(size_t capacity=0);
    /** Returns the underlying byte array **/
    virtual const std::vector<uint8_t> &getBuffer() const { return buffer; }
    /** Write a portion of data **/
    void write(const void * from, size_t length) override;
    
private:
    std::vector<uint8_t> buffer;
};

/** Writer for buffered data **/
class ByteArrayRefWriter : public Writer {
public:
    /**/
    ByteArrayRefWriter(std::vector<uint8_t> &buffer);
    /** Returns the underlying byte array **/
    virtual const std::vector<uint8_t> &getBuffer() const { return buffer; }
    /** Write a portion of data **/
    void write(const void * from, size_t length) override;
    
private:
    std::vector<uint8_t> &buffer;
};

}

#endif
