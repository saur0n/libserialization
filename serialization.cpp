/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2019, Sauron
 ******************************************************************************/

#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "serialization.hpp"

using namespace rohan;
using std::system_error;
using std::vector;

unsigned long long rohan::readVariableInteger(InputStream &stream) {
    unsigned long long result=0;
    uint8_t byte=0x80;
    unsigned shift=0;
    while (byte&0x80) {
        stream | byte;
        result|=((unsigned long long)(byte&0x7f)<<shift);
        shift+=7;
    }
    return result;
}

void rohan::writeVariableInteger(OutputStream &stream, unsigned long long value) {
    uint8_t len=0, buffer[16];
    while (value>=0x80) {
        buffer[len++]=0x80|(0x7f&value);
        value>>=7;
    }
    buffer[len++]=value;
    stream.write(buffer, len);
}

signed long long rohan::readSignedVariableInteger(InputStream &stream) {
    unsigned long long tmp=readVariableInteger(stream);
    return ((1&tmp)?(tmp^(~0)):tmp)>>1;
}

void rohan::writeSignedVariableInteger(OutputStream &stream, signed long long value) {
    writeVariableInteger(stream, value>=0?(value<<1):(value<<1)^(~0));
}

OutputStream &rohan::operator |(OutputStream &stream, const char * string) {
    size_t length=strlen(string);
    stream | length;
    stream.write(string, length);
    return stream;
}

OutputStream &rohan::operator |(OutputStream &stream, const wchar_t * string) {
    size_t length=wcslen(string);
    stream | length;
    return writeArray(stream, string, length);
}

/******************************************************************************/

ByteArrayInputStream::ByteArrayInputStream(const vector<uint8_t> &buffer,
        size_t offset) : buffer(buffer), offset(offset) {}

void ByteArrayInputStream::read(void * to, size_t length) {
    if (buffer.size()<offset+length)
        throw End();
    memcpy(to, &buffer[offset], length);
    offset+=length;
}

/******************************************************************************/

ByteArrayOutputStream::ByteArrayOutputStream(vector<uint8_t> &buffer) :
        buffer(buffer) {}

void ByteArrayOutputStream::write(const void * from, size_t length) {
    size_t oldSize=buffer.size();
    buffer.resize(oldSize+length);
    memcpy(&buffer[oldSize], from, length);
}

/******************************************************************************/

void FileInputStream::read(void * to, size_t length) {
    size_t nr=0;
    while (nr<length) {
        auto retval=::read(fd, reinterpret_cast<uint8_t *>(to)+nr, length);
        if (retval<0)
            throw system_error(errno, std::generic_category());
        else if (retval==0)
            throw End();
        else
            nr+=retval;
    }
}

/******************************************************************************/

void FileOutputStream::write(const void * buffer, size_t length) {
    auto retval=::write(fd, buffer, length);
    if (retval<0)
        throw system_error(errno, std::generic_category());
}
