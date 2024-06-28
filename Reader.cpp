/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2024, Sauron
 ******************************************************************************/

#include "Reader.hpp"

using namespace rohan;

/******************************************************************************/

unsigned long long rohan::readVariableInteger(Reader &stream) {
    unsigned long long result=0;
    uint8_t byte=0x80;
    unsigned shift=0;
    while (byte&0x80) {
        byte=uint8_t(stream);
        result|=((unsigned long long)(byte&0x7f)<<shift);
        shift+=7;
    }
    return result;
}

signed long long rohan::readSignedVariableInteger(Reader &stream) {
    unsigned long long tmp=readVariableInteger(stream);
    return ((1&tmp)?(tmp^(~0)):tmp)>>1;
}
