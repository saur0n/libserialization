/*******************************************************************************
 *  Rohan data serialization library.
 *  Reader with bufferization
 *  
 *  © 2023, Sauron
 ******************************************************************************/

#include <cstring>
#include <stdexcept>
#include "BufferedReader.hpp"

using namespace rohan;

/******************************************************************************/

BufferedReader::BufferedReader(Reader &base, size_t bufferSize) :
        base(base), bufferSize(bufferSize), position(0) {
    if (!bufferSize)
        throw std::invalid_argument("bufferSize");
}

short BufferedReader::read() {
    if (position>=buffer.size()) {
        // Read the next portion of data
        populate();
    }
    if (position<buffer.size())
        return buffer[position++];
    else
        return -1;
}

size_t BufferedReader::read(void * to, size_t length) {
    size_t result=0;
    
    if (length) {
        uint8_t * destination=reinterpret_cast<uint8_t *>(to);
        size_t available=buffer.size()-position;
        
        if (available<length) {
            if (available) {
                // Scratch out the buffer
                memcpy(destination, &buffer[position], available);
                position=buffer.size();
                destination+=available;
                length-=available;
                result=available;
            }
            if (length>bufferSize) {
                // Do not use the buffer
                result+=base.read(destination, length);
            }
            else {
                // Read the data, and return part of them to the caller
                populate();
                if (length>buffer.size())
                    length=buffer.size();
                memcpy(destination, buffer.data(), length);
                position+=length;
                result+=length;
            }
            return result;
        }
        else {
            memcpy(destination, &buffer[position], length);
            position+=length;
            result=length;
        }
    }
    
    return result;
}

void BufferedReader::populate() {
    buffer.resize(bufferSize);
    size_t length=base.read(&buffer[0], buffer.size());
    if (length!=buffer.size())
        buffer.resize(length);
    position=0;
}

