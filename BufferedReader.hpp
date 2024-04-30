/*******************************************************************************
 *  Rohan data serialization library.
 *  Reader with bufferization
 *  
 *  © 2023, Sauron
 ******************************************************************************/

#ifndef __ROHAN_BUFFEREDREADER_HPP
#define __ROHAN_BUFFEREDREADER_HPP

#include "Serialization.hpp"

namespace rohan {

/**/
class BufferedReader : public Reader {
public:
    /** Create a buffered reader **/
    BufferedReader(Reader &base, size_t bufferSize);
    /** Returns the underlying stream **/
    Reader &getBase() const { return base; }
    /** Returns the buffer size **/
    size_t getBufferSize() const { return bufferSize; }
    /** Read a byte of data **/
    short read();
    /** Read a portion of data **/
    size_t read(void * to, size_t length) override;
    
private:
    void populate();
    
    Reader &base;
    size_t bufferSize;
    size_t position;
    std::vector<uint8_t> buffer;
};

}

#endif
