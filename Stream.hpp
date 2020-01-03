/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2019, Sauron
 ******************************************************************************/

#ifndef __SERIALIZATION_HPP
#define __SERIALIZATION_HPP

#include <array>
#include <cstdint>
#include <cwchar>
#include <map>
#include <string>
#include <system_error>
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
    value=static_cast<type>(readVariableInteger(stream)); \
    return stream; \
} \
inline OutputStream &operator |(OutputStream &stream, const type &value) { \
    writeVariableInteger(stream, value); \
    return stream; \
}

#define _RW_ZIGZAG(type) \
inline InputStream &operator |(InputStream &stream, type &value) { \
    value=static_cast<type>(readSignedVariableInteger(stream)); \
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
_RW_LEB128(wchar_t)
_RW_ZIGZAG(int16_t)
_RW_LEB128(uint16_t)
_RW_ZIGZAG(int32_t)
_RW_LEB128(uint32_t)
_RW_ZIGZAG(int64_t)
_RW_LEB128(uint64_t)
_RW_ZIGZAG(long long)
_RW_LEB128(unsigned long long)
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

template <class K, class V>
InputStream &operator |(InputStream &stream, std::map<K, V> &map) {
    size_t length=readVariableInteger(stream);
    map.clear();
    for (size_t i=0; i<length; i++) {
        std::pair<K, V> pair;
        stream | pair;
        map.insert(pair);
    }
    return stream;
}

template <class K, class V>
OutputStream &operator |(OutputStream &stream, const std::map<K, V> &map) {
    writeVariableInteger(stream, map.size());
    for (auto i=map.begin(); i!=map.end(); ++i)
        stream | *i;
    return stream;
}

OutputStream &operator |(OutputStream &stream, const char * string);

OutputStream &operator |(OutputStream &stream, const wchar_t * string);

/*******************************************************************************
 *  INPUT/OUTPUT STREAM IMPEMENTATIONS
 ******************************************************************************/

/** Thrown at end-of-file **/
class End {};

/** Reader for buffered data **/
class ByteArrayInputStream : public InputStream {
public:
    /**/
    ByteArrayInputStream(const std::vector<uint8_t> &buffer, size_t offset=0);
    /** Returns underlying byte array **/
    const std::vector<uint8_t> &getBuffer() const { return buffer; }
    /**/
    void read(void * to, size_t length);
    
private:
    const std::vector<uint8_t> &buffer;
    size_t offset;
};

/** Writer for buffered data **/
class ByteArrayOutputStream : public OutputStream {
public:
    /**/
    ByteArrayOutputStream(std::vector<uint8_t> &buffer);
    /** Returns underlying byte array **/
    std::vector<uint8_t> &getBuffer() const { return buffer; }
    /**/
    void write(const void * from, size_t length);
    
private:
    std::vector<uint8_t> &buffer;
};

/** Reader for file descriptors **/
class FileInputStream : public InputStream {
public:
    /**/
    FileInputStream(int fd) : fd(fd) {}
    /**/
    void read(void * to, size_t length);
    
private:
    int fd;
};

/** Writer for file descriptors **/
class FileOutputStream : public OutputStream {
public:
    /**/
    FileOutputStream(int fd) : fd(fd) {}
    /**/
    void write(const void * buffer, size_t length);
    
private:
    int fd;
};

}

#endif
