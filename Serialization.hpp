/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2021, Sauron
 ******************************************************************************/

#ifndef __ROHAN_SERIALIZATION_HPP
#define __ROHAN_SERIALIZATION_HPP

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
class Reader {
public:
    /** Virtual destructor **/
    virtual ~Reader() {}
    /** Read a portion of data **/
    virtual void read(void * to, size_t length)=0;
    /** Unserialize a value using procedural style **/
    template <class T>
    inline explicit operator T() {
        T result;
        *this | result;
        return result;
    }
};

/** Abstract data sink **/
class Writer {
public:
    /** Virtual destructor **/
    virtual ~Writer() {}
    /** Write a portion of data **/
    virtual void write(const void * from, size_t length)=0;
    /** Write several values at once **/
    template <class T, class... A>
    void put(T&& first, A&&... rest) {
        *this | first;
        put(rest...);
    }
    
private:
    void put() {}
};

unsigned long long readVariableInteger(Reader &stream);
void writeVariableInteger(Writer &stream, unsigned long long value);
signed long long readSignedVariableInteger(Reader &stream);
void writeSignedVariableInteger(Writer &stream, signed long long value);

#define _RW_FIXED(type) \
inline Reader &operator |(Reader &stream, type &value) { \
    stream.read(&value, sizeof(value)); \
    return stream; \
} \
inline Writer &operator |(Writer &stream, const type &value) { \
    stream.write(&value, sizeof(value)); \
    return stream; \
}

#define _RW_LEB128(type) \
inline Reader &operator |(Reader &stream, type &value) { \
    value=static_cast<type>(readVariableInteger(stream)); \
    return stream; \
} \
inline Writer &operator |(Writer &stream, const type &value) { \
    writeVariableInteger(stream, value); \
    return stream; \
}

#define _RW_ZIGZAG(type) \
inline Reader &operator |(Reader &stream, type &value) { \
    value=static_cast<type>(readSignedVariableInteger(stream)); \
    return stream; \
} \
inline Writer &operator |(Writer &stream, const type &value) { \
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
inline Reader &readArray(Reader &stream, T &array, size_t n) {
    for (size_t i=0; i<n; i++)
        stream | array[i];
    return stream;
}

template <class T>
inline Writer &writeArray(Writer &stream, const T &array, size_t n) {
    for (size_t i=0; i<n; i++)
        stream | array[i];
    return stream;
}

template <class T, class = decltype(&T::serialize)>
inline Writer &operator |(Writer &stream, const T &value) {
    value.serialize(stream);
    return stream;
}

template <class T, size_t n>
Reader &operator |(Reader &stream, T (&value)[n]) {
    return readArray(stream, value, n);
}

template <class T, size_t n>
Writer &operator |(Writer &stream, const T (&value)[n]) {
    return writeArray(stream, value, n);
}

template <class T, size_t n>
Reader &operator |(Reader &stream, std::array<T, n> &array) {
    return readArray(stream, array, n);
}

template <class T, size_t n>
Writer &operator |(Writer &stream, std::array<T, n> &array) {
    return writeArray(stream, array, n);
}

template <class T>
Reader &operator |(Reader &stream, std::basic_string<T> &string) {
    string.resize(readVariableInteger(stream));
    return readArray(stream, string, string.length());
}

template <class T>
Writer &operator |(Writer &stream, const std::basic_string<T> &string) {
    size_t length=string.length();
    writeVariableInteger(stream, length);
    return writeArray(stream, string, length);
}

template <class T, typename std::enable_if<std::is_constructible<T, Reader &>::value, int>::type=0>
Reader &operator |(Reader &stream, std::vector<T> &vector) {
    size_t n=readVariableInteger(stream);
    for (size_t i=0; i<n; i++)
        vector.emplace_back(stream);
    return stream;
}

template <class T, typename std::enable_if<std::is_default_constructible<T>::value && !std::is_constructible<T, Reader &>::value, int>::type=0>
Reader &operator |(Reader &stream, std::vector<T> &vector) {
    vector.resize(readVariableInteger(stream));
    return readArray(stream, vector, vector.size());
}

template <class T>
Writer &operator |(Writer &stream, const std::vector<T> &vector) {
    size_t length=vector.size();
    writeVariableInteger(stream, length);
    return writeArray(stream, vector, length);
}

template <class X, class Y>
Reader &operator |(Reader &stream, std::pair<X, Y> &value) {
    return stream | value.first | value.second;
}

template <class X, class Y>
Writer &operator |(Writer &stream, const std::pair<X, Y> &value) {
    return stream | value.first | value.second;
}

template <class K, class V>
Reader &operator |(Reader &stream, std::map<K, V> &map) {
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
Writer &operator |(Writer &stream, const std::map<K, V> &map) {
    writeVariableInteger(stream, map.size());
    for (auto i=map.begin(); i!=map.end(); ++i)
        stream | *i;
    return stream;
}

Writer &operator |(Writer &stream, const char * string);

Writer &operator |(Writer &stream, const wchar_t * string);

/*******************************************************************************
 *  COMPARISON OPERATORS
 ******************************************************************************/

template <class T>
bool __equals(const T &a, const T &b) {
    return a==b;
}

template <class T, size_t n>
bool __equals(const T (&a)[n], const T (&b)[n]) {
    for (size_t i=0; i<n; i++)
        if (!__equals(a[i], b[i]))
            return false;
    return true;
}

template <class T>
bool operator ==(Reader &reader, const T &refValue) {
    T value;
    reader | value;
    return __equals(value, refValue);
}

template <class T>
bool operator !=(Reader &reader, const T &refValue) {
    T value;
    reader | value;
    return !__equals(value, refValue);
}

/*******************************************************************************
 *  INPUT/OUTPUT STREAM IMPEMENTATIONS
 ******************************************************************************/

/** Thrown at end-of-file **/
class End {};

/** Reader for file descriptors **/
class FileReader : public Reader {
public:
    /**/
    FileReader(int fd) : fd(fd) {}
    /**/
    void read(void * to, size_t length);
    
private:
    int fd;
};

/** Writer for file descriptors **/
class FileWriter : public Writer {
public:
    /**/
    FileWriter(int fd) : fd(fd) {}
    /**/
    void write(const void * buffer, size_t length);
    
private:
    int fd;
};

}

#endif
