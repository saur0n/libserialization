/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2023, Sauron
 ******************************************************************************/

#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "Serialization.hpp"

using namespace rohan;
using std::system_error;
using std::vector;

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

void rohan::writeVariableInteger(Writer &stream, unsigned long long value) {
    uint8_t len=0, buffer[16];
    while (value>=0x80) {
        buffer[len++]=0x80|(0x7f&value);
        value>>=7;
    }
    buffer[len++]=value;
    stream.write(buffer, len);
}

signed long long rohan::readSignedVariableInteger(Reader &stream) {
    unsigned long long tmp=readVariableInteger(stream);
    return ((1&tmp)?(tmp^(~0)):tmp)>>1;
}

void rohan::writeSignedVariableInteger(Writer &stream, signed long long value) {
    writeVariableInteger(stream, value>=0?(value<<1):(value<<1)^(~0));
}

Writer &rohan::operator |(Writer &stream, const char * string) {
    size_t length=strlen(string);
    stream | length;
    stream.write(string, length);
    return stream;
}

Writer &rohan::operator |(Writer &stream, const wchar_t * string) {
    size_t length=wcslen(string);
    stream | length;
    for (size_t i=0; i<length; i++)
        stream | string[i];
    return stream;
}

