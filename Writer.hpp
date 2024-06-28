/*******************************************************************************
 *  Rohan data serialization library.
 *  
 *  © 2016—2024, Sauron
 ******************************************************************************/

#ifndef __ROHAN_WRITER_HPP
#define __ROHAN_WRITER_HPP

#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

namespace rohan {

inline uint8_t  operator ""_b(unsigned long long int value) { return value; }
inline uint16_t operator ""_s(unsigned long long int value) { return value; }
inline uint32_t operator ""_u(unsigned long long int value) { return value; }
inline uint64_t operator ""_l(unsigned long long int value) { return value; }

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

void writeVariableInteger(Writer &stream, unsigned long long value);

void writeSignedVariableInteger(Writer &stream, signed long long value);

template <class T>
inline typename std::make_unsigned<T>::type _encodeZigzag(T value) {
    T vshift=value<<1;
    return value>=0?vshift:vshift^(T(-1));
}

#define _W_FIXED(T) \
    inline Writer &operator |(Writer &stream, const T &value) { \
        stream.write(&value, sizeof(value)); \
        return stream; \
    }

#define _W_LEB128(T) \
    inline Writer &operator |(Writer &stream, const T &value) { \
        writeVariableInteger(stream, value); \
        return stream; \
    }

#define _W_ZIGZAG(T) \
    inline Writer &operator |(Writer &stream, const T &value) { \
        writeVariableInteger(stream, _encodeZigzag<T>(value)); \
        return stream; \
    }

_W_FIXED(bool)
_W_FIXED(char)
_W_FIXED(int8_t)
_W_FIXED(uint8_t)
_W_LEB128(wchar_t)
_W_ZIGZAG(int16_t)
_W_LEB128(uint16_t)
_W_ZIGZAG(int32_t)
_W_LEB128(uint32_t)
_W_ZIGZAG(int64_t)
_W_LEB128(uint64_t)
#if __LONG_WIDTH__==64
_W_ZIGZAG(long long)
_W_LEB128(unsigned long long)
#endif
_W_FIXED(float)
_W_FIXED(double)
_W_FIXED(long double)

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
Writer &operator |(Writer &stream, const std::array<T, n> &value) {
    for (size_t i=0; i<n; i++)
        stream | value[i];
    return stream;
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
Writer &operator |(Writer &stream, const std::vector<T> &vector) {
    size_t length=vector.size();
    writeVariableInteger(stream, length);
    for (size_t i=0; i<length; i++)
        stream | vector[i];
    return stream;
}

template <class X, class Y>
Writer &operator |(Writer &stream, const std::pair<X, Y> &value) {
    return stream | value.first | value.second;
}

template <class K, class V>
Writer &operator |(Writer &stream, const std::map<K, V> &map) {
    writeVariableInteger(stream, map.size());
    for (auto i=map.begin(); i!=map.end(); ++i)
        stream | *i;
    return stream;
}

template <class T>
Writer &operator |(Writer &stream, const std::set<T> &set) {
    size_t length=set.size();
    writeVariableInteger(stream, length);
    for (auto i=set.begin(); i!=set.end(); ++i)
        stream | *i;
    return stream;
}

template<class T, typename = std::enable_if_t<std::is_enum_v<T>>>
Writer &operator |(Writer &stream, T value) {
    return stream | uint8_t(value);
}

Writer &operator |(Writer &stream, const char * string);

Writer &operator |(Writer &stream, const wchar_t * string);

}

#endif
