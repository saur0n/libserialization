/*******************************************************************************
 *  Rohan data serialization library.
 *  Serialization to unix++ streams
 * 
 *  © 2020—2024, Sauron
 ******************************************************************************/

#ifndef __ROHAN_STREAMSERIALIZATION_HPP 
#define __ROHAN_STREAMSERIALIZATION_HPP

#include <unix++/Stream.hpp>
#include "Serialization.hpp"

namespace rohan {

/** Base class for StreamReader and StreamWriter **/
class StreamSerializer {
public:
    /** Initialize with a reference to the stream **/
    explicit StreamSerializer(upp::Stream &stream);
    /** Returns the underlying stream **/
    upp::Stream &getStream() const { return stream; }
    
protected:
    upp::Stream &stream;
};

/**/
class StreamReader : public Reader, StreamSerializer {
public:
    /** Initialize with a reference to the stream **/
    explicit StreamReader(upp::Stream &stream);
    /** Read a portion of data **/
    size_t read(void * to, size_t length) override;
    /** Skip a portion of data **/
    size_t skip(size_t length) override;
};

class StreamWriter : public Writer, StreamSerializer {
public:
    /** Initialize with a reference to the stream **/
    explicit StreamWriter(upp::Stream &stream);
    /** Write a portion of data **/
    void write(const void * from, size_t length) override;
};

}

#endif
