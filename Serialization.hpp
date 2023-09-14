/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2023, Sauron
 ******************************************************************************/

#ifndef __ROHAN_SERIALIZATION_HPP
#define __ROHAN_SERIALIZATION_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <cwchar>
#include <map>
#include <set>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace rohan {

inline uint8_t  operator ""_b(unsigned long long int value) { return value; }
inline uint16_t operator ""_s(unsigned long long int value) { return value; }
inline uint32_t operator ""_u(unsigned long long int value) { return value; }
inline uint64_t operator ""_l(unsigned long long int value) { return value; }

template <class T>
inline T _read(class Reader &stream) {
    static_assert("wrong data type");
}

/** This exception indicates that end of file or stream was reached **/
class End {};

/** Abstract data source **/
class Reader {
public:
    /** Virtual destructor **/
    virtual ~Reader() {}
    /** Read a portion of data **/
    virtual size_t read(void * to, size_t length)=0;
    /** Read a portion of data, throw End() if could not be read completely **/
    virtual void readFully(void * to, size_t length);
    /** Unserialize a value using "type conversion" style **/
    template <class T, typename std::enable_if<std::is_constructible<T, Reader &>::value, int>::type=0>
    inline explicit operator T() {
        return T(*this);
    }
    /** Unserialize an object using "type conversion" style **/
    template <class T, typename std::enable_if<!std::is_constructible<T, Reader &>::value, int>::type=0>
    inline explicit operator T() {
        return _read(*this, static_cast<T *>(nullptr));
    }
    /** Read one or more default-constructible values **/
    template <class T, class... A>
    void get(T& first, A&... rest) {
        first=T(*this);
        get(rest...);
    }
    
private:
    void get();
};

inline void Reader::readFully(void * to, size_t length) {
    if (length!=read(to, length))
        throw End();
}

/** Abstract data sink **/
class Writer {
public:
    /** Virtual destructor **/
    virtual ~Writer() {}
    /** Write a portion of data **/
    virtual void write(const void * from, size_t length)=0;
    /** Write one or more values at once **/
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

template <class T>
inline T _decodeZigzag(T value) {
    typename std::make_unsigned<T>::type _value=value;
    return (_value>>1)^-(_value&1);
}

template <class T>
inline typename std::make_unsigned<T>::type _encodeZigzag(T value) {
    T vshift=value<<1;
    return value>=0?vshift:vshift^(T(-1));
}

#define _RW_FIXED(T) \
    inline T _read(Reader &stream, T * dummy=nullptr) { \
        (void)dummy; \
        T result; \
        stream.readFully(&result, sizeof(result)); \
        return result; \
    } \
    inline Writer &operator |(Writer &stream, const T &value) { \
        stream.write(&value, sizeof(value)); \
        return stream; \
    }

#define _RW_LEB128(T) \
    inline T _read(Reader &stream, T * dummy=nullptr) { \
        (void)dummy; \
        return static_cast<T>(readVariableInteger(stream)); \
    } \
    inline Writer &operator |(Writer &stream, const T &value) { \
        writeVariableInteger(stream, value); \
        return stream; \
    }

#define _RW_ZIGZAG(T) \
    inline T _read(Reader &stream, T * dummy=nullptr) { \
        (void)dummy; \
        return _decodeZigzag<T>(readVariableInteger(stream)); \
    } \
    inline Writer &operator |(Writer &stream, const T &value) { \
        writeVariableInteger(stream, _encodeZigzag<T>(value)); \
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
#if __LONG_WIDTH__==64
_RW_ZIGZAG(long long)
_RW_LEB128(unsigned long long)
#endif
_RW_FIXED(float)
_RW_FIXED(double)
_RW_FIXED(long double)

template <class T, class = decltype(&T::serialize)>
inline Writer &operator |(Writer &stream, const T &value) {
    value.serialize(stream);
    return stream;
}

template <class T, size_t n>
Writer &operator |(Writer &stream, const T (&value)[n]) {
    for (size_t i=0; i<n; i++)
        stream | value[i];
    return stream;
}

template <class T, size_t n>
std::array<T, n> _read(Reader &stream, std::array<T, n> * dummy) {
    (void)dummy;
    std::array<T, n> result;
    for (size_t i=0; i<n; i++)
        result[i]=T(stream);
    return result;
}

template <class T, size_t n>
Writer &operator |(Writer &stream, const std::array<T, n> &value) {
    for (size_t i=0; i<n; i++)
        stream | value[i];
    return stream;
}

template <class T>
std::basic_string<T> _read(Reader &stream, std::basic_string<T> * dummy) {
    (void)dummy;
    const size_t PAGE_SIZE=4096;
    std::basic_string<T> result;
    size_t n=readVariableInteger(stream);
    result.reserve(std::min(PAGE_SIZE, n));
    while (n-->0)
        result.push_back(T(stream));
    return result;
}

template <class T>
Writer &operator |(Writer &stream, const std::basic_string<T> &string) {
    size_t length=string.length();
    writeVariableInteger(stream, length);
    for (size_t i=0; i<length; i++)
        stream | string[i];
    return stream;
}

template <class T>
std::vector<T> _read(Reader &stream, std::vector<T> * dummy) {
    (void)dummy;
    std::vector<T> result;
    size_t n=readVariableInteger(stream);
    for (size_t i=0; i<n; i++)
        result.emplace_back(stream);
    return result;
}

template <class T>
Writer &operator |(Writer &stream, const std::vector<T> &vector) {
    size_t length=vector.size();
    writeVariableInteger(stream, length);
    for (size_t i=0; i<length; i++)
        stream | vector[i];
    return stream;
}

template <class X, class Y>
std::pair<X, Y> _read(Reader &stream, std::pair<X, Y> * dummy) {
    (void)dummy;
    return std::pair<X, Y>{X(stream), Y(stream)};
}

template <class X, class Y>
Writer &operator |(Writer &stream, const std::pair<X, Y> &value) {
    return stream | value.first | value.second;
}

template <class K, class V>
std::map<K, V> _read(Reader &stream, std::map<K, V> * dummy) {
    (void)dummy;
    std::map<K, V> result;
    size_t length=readVariableInteger(stream);
    for (size_t i=0; i<length; i++)
        result.insert(std::pair<K, V>(stream));
    return result;
}

template <class K, class V>
Writer &operator |(Writer &stream, const std::map<K, V> &map) {
    writeVariableInteger(stream, map.size());
    for (auto i=map.begin(); i!=map.end(); ++i)
        stream | *i;
    return stream;
}

template <class T>
std::set<T> _read(Reader &stream, std::set<T> * dummy) {
    (void)dummy;
    std::set<T> result;
    size_t length=readVariableInteger(stream);
    while (length-->0)
        result.emplace(stream);
    return result;
}

template <class T>
Writer &operator |(Writer &stream, const std::set<T> &set) {
    size_t length=set.size();
    writeVariableInteger(stream, length);
    for (auto i=set.begin(); i!=set.end(); ++i)
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

}

#endif
