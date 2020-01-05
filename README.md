# libserialization

This library allows to serialize standard C++ data types to binary form.

## Supported data types

Serializing and deserializing of the following data types is supported:
* `bool`
* `char`
* `int8_t`
* `uint8_t`
* `int16_t`
* `uint16_t`
* `int32_t`
* `uint32_t`
* `int64_t`
* `uint64_t`
* `long long` (not the same as `int64_t`)
* `unsigned long long` (not the same as `uint64_t`)
* fixed-size arrays
* `std::array`
* `std::basic_string`
* `std::pair`

Serialization of the following data types is supported:
* `const char *` (C-strings)
* any classes with `serialize()` method
* `std::vector`
* `std::map`

Deserialization of the following data types is supported:
* `std::vector<T>`, where T is default constructible or `InputStream` constructible
* `std::map<K, V>`, where K and V are both default constructible

## Usage

The following include statement is necessary:
```
#include <rohan/Stream.hpp>
```

Optional `using` directives:
```
using rohan::InputStream;
using rohan::OutputStream;
```
