/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2019, Sauron
 ******************************************************************************/

#ifndef __STREAM_HPP
#define __STREAM_HPP

#include <array>
#include <cstdint>
#include <cwchar>
#include <string>
#include <utility>
#include <vector>

namespace rohan {

/** Abstract data source **/
class InputStream {
public:
    virtual ~InputStream() {}
    virtual void read(void * to, size_t length)=0;
};

/** Abstract data sink **/
class OutputStream {
public:
    virtual ~OutputStream() {}
    virtual void write(const void * from, size_t length)=0;
};

unsigned long long readVariableInteger(InputStream &stream);
void writeVariableInteger(OutputStream &stream, unsigned long long value);
signed long long readSignedVariableInteger(InputStream &stream);
void writeSignedVariableInteger(OutputStream &stream, signed long long value);

#define _RW_FIXED(type) \
inline InputStream &operator |(InputStream &stream, type &value) { \
    stream.read(&value, sizeof(value)); \
    return stream; \
} \
inline OutputStream &operator |(OutputStream &stream, const type &value) { \
    stream.write(&value, sizeof(value)); \
    return stream; \
}

#define _RW_LEB128(type) \
inline InputStream &operator |(InputStream &stream, type &value) { \
    value=type(readVariableInteger(stream)); \
    return stream; \
} \
inline OutputStream &operator |(OutputStream &stream, const type &value) { \
    writeVariableInteger(stream, value); \
    return stream; \
}

#define _RW_ZIGZAG(type) \
inline InputStream &operator |(InputStream &stream, type &value) { \
    value=type(readSignedVariableInteger(stream)); \
    return stream; \
} \
inline OutputStream &operator |(OutputStream &stream, const type &value) { \
    writeSignedVariableInteger(stream, value); \
    return stream; \
}

_RW_FIXED(bool)
_RW_FIXED(char)
_RW_FIXED(int8_t)
_RW_FIXED(uint8_t)
_RW_LEB128(wchar_t);
_RW_ZIGZAG(int16_t)
_RW_LEB128(uint16_t)
_RW_ZIGZAG(int32_t)
_RW_LEB128(uint32_t)
_RW_ZIGZAG(int64_t)
_RW_LEB128(uint64_t)
_RW_FIXED(float)
_RW_FIXED(double)
_RW_FIXED(long double)

template <class T>
inline InputStream &readArray(InputStream &stream, T &array, size_t n) {
    for (size_t i=0; i<n; i++)
        stream | array[i];
    return stream;
}

template <class T>
inline OutputStream &writeArray(OutputStream &stream, const T &array, size_t n) {
    for (size_t i=0; i<n; i++)
        stream | array[i];
    return stream;
}

template <class T, size_t n>
InputStream &operator |(InputStream &stream, T (&value)[n]) {
    return readArray(stream, value, n);
}

template <class T, size_t n>
OutputStream &operator |(OutputStream &stream, const T (&value)[n]) {
    return writeArray(stream, value, n);
}

template <class T, size_t n>
InputStream &operator |(InputStream &stream, std::array<T, n> &array) {
    return readArray(stream, array, n);
}

template <class T, size_t n>
OutputStream &operator |(OutputStream &stream, std::array<T, n> &array) {
    return writeArray(stream, array, n);
}

template <class T>
InputStream &operator |(InputStream &stream, std::basic_string<T> &string) {
    string.resize(readVariableInteger(stream));
    return readArray(stream, string, string.length());
}

template <class T>
OutputStream &operator |(OutputStream &stream, const std::basic_string<T> &string) {
    size_t length=string.length();
    writeVariableInteger(stream, length);
    return writeArray(stream, string, length);
}

template <class T>
InputStream &operator |(InputStream &stream, std::vector<T> &vector) {
    vector.resize(readVariableInteger(stream));
    return readArray(stream, vector, vector.size());
}

template <class T>
OutputStream &operator |(OutputStream &stream, const std::vector<T> &vector) {
    size_t length=vector.size();
    writeVariableInteger(stream, length);
    return writeArray(stream, vector, length);
}

template <class X, class Y>
InputStream &operator |(InputStream &stream, std::pair<X, Y> &value) {
    return stream | value.first | value.second;
}

template <class X, class Y>
OutputStream &operator |(OutputStream &stream, const std::pair<X, Y> &value) {
    return stream | value.first | value.second;
}

OutputStream &operator |(OutputStream &stream, const char * string);

OutputStream &operator |(OutputStream &stream, const wchar_t * string);

}

#endif