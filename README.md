# libserialization

This library allows to serialize standard C++ data types to binary form.

## Supported data types

Serialization and deserialization of the following data types is supported:
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
* `std::array`
* `std::basic_string`
* `std::list`
* `std::map`
* `std::pair`
* `std::set`
* `std::vector`

Serialization of the following data types is supported:
* fixed-size arrays
* `const char *` (C-strings)
* any classes that have `serialize()` method (see below)

Deserialization of the following data types is supported:
* any custom classes which have deserialization constructor (see below)

Serialization and deserialization of `float`, `double` and `long double` are implemented, but data format will be changed in future.

## Usage

The following include statement is necessary:
```
#include <rohan/Reader.hpp>
```

Optional `using` directives:
```
using rohan::Reader;
using rohan::Writer;
```

### Writing data
Data is written using the `|` operator:
```
const unsigned magic=0x4698CD67;
std::vector<std::string> vec {"alpha", "beta", "gamma"};
writer | magic | vec | "string";
```

To write fields of a class, add constant method `serialize()`:
```
class Color {
public:
    explicit Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    void serialize(Writer &writer) const {
        writer | r | g | b;
    }
private:
    uint8_t r, g, b;
};
```

### Reading data
Data is read from the stream using type conversion operator:
```
unsigned nCows=unsigned(reader);
std::vector<std::string> vec=std::vector<std::string>(reader);
```

To read fields of a class, create a unserialization constructor:
```
class Color {
public:
    explicit Color(Reader &reader) :
        r(uint8_t(reader)),
        g(uint8_t(reader)),
        b(uint8_t(reader)) {}
private:
    uint8_t r, g, b;
};
```
