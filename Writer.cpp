/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2024, Sauron
 ******************************************************************************/

#include <cstring>
#include "Writer.hpp"

using namespace rohan;

/******************************************************************************/

void rohan::writeVariableInteger(Writer &stream, unsigned long long value) {
    uint8_t len=0, buffer[16];
    while (value>=0x80) {
        buffer[len++]=0x80|(0x7f&value);
        value>>=7;
    }
    buffer[len++]=value;
    stream.write(buffer, len);
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
