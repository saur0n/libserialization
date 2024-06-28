/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2024, Sauron
 ******************************************************************************/

#ifndef __ROHAN_READER_HPP
#define __ROHAN_READER_HPP

#include <cstdint>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace rohan {

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
    /** Skip a portion of data **/
    virtual size_t skip(size_t length)=0;
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

unsigned long long readVariableInteger(Reader &stream);

signed long long readSignedVariableInteger(Reader &stream);

template <class T>
inline T _decodeZigzag(T value) {
    typename std::make_unsigned<T>::type _value=value;
    return (_value>>1)^-(_value&1);
}

#define _R_FIXED(T) \
    inline T _read(Reader &stream, T * dummy=nullptr) { \
        (void)dummy; \
        T result; \
        stream.readFully(&result, sizeof(result)); \
        return result; \
    }

#define _R_LEB128(T) \
    inline T _read(Reader &stream, T * dummy=nullptr) { \
        (void)dummy; \
        return static_cast<T>(readVariableInteger(stream)); \
    }

#define _R_ZIGZAG(T) \
    inline T _read(Reader &stream, T * dummy=nullptr) { \
        (void)dummy; \
        return _decodeZigzag<T>(readVariableInteger(stream)); \
    }

_R_FIXED(bool)
_R_FIXED(char)
_R_FIXED(int8_t)
_R_FIXED(uint8_t)
_R_LEB128(wchar_t)
_R_ZIGZAG(int16_t)
_R_LEB128(uint16_t)
_R_ZIGZAG(int32_t)
_R_LEB128(uint32_t)
_R_ZIGZAG(int64_t)
_R_LEB128(uint64_t)
#if __LONG_WIDTH__==64
_R_ZIGZAG(long long)
_R_LEB128(unsigned long long)
#endif
_R_FIXED(float)
_R_FIXED(double)
_R_FIXED(long double)

template <class T, size_t n>
std::array<T, n> _read(Reader &stream, std::array<T, n> * dummy) {
    (void)dummy;
    std::array<T, n> result;
    for (size_t i=0; i<n; i++)
        result[i]=T(stream);
    return result;
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
std::list<T> _read(Reader &stream, std::list<T> * dummy) {
    (void)dummy;
    std::list<T> result;
    size_t n=readVariableInteger(stream);
    for (size_t i=0; i<n; i++)
        result.emplace_back(stream);
    return result;
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

template <class X, class Y>
std::pair<X, Y> _read(Reader &stream, std::pair<X, Y> * dummy) {
    (void)dummy;
    return std::pair<X, Y>{X(stream), Y(stream)};
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

template <class T>
std::set<T> _read(Reader &stream, std::set<T> * dummy) {
    (void)dummy;
    std::set<T> result;
    size_t length=readVariableInteger(stream);
    while (length-->0)
        result.emplace(stream);
    return result;
}

template<class T, typename = std::enable_if_t<std::is_enum_v<T>>>
T _read(Reader &stream, T * dummy) {
    (void)dummy;
    return T(uint8_t(stream));
}

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
