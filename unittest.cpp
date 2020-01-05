/*******************************************************************************
 *  Rohan data serialization library
 *  Unit tests
 *  
 *  © 2016—2019, Sauron
 ******************************************************************************/

#include <cerrno>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "Stream.hpp"

using namespace rohan;
using namespace std;

class File {
public:
    File(const char * filename, int mode=O_RDONLY, int rights=0) : fd(open(filename, mode, rights)) {
        if (fd<0)
            throw errno;
    }
    virtual ~File() {
        close(fd);
    }
    int getDescriptor() const { return fd; }
    
protected:
    File(File &)=delete;
    
    int fd;
};

/*
 *  This class is an example of usage of the serialize() method.
 */
class Variant {
public:
    explicit Variant(bool boolean) : type(boolean?1:0), number(0) {}
    explicit Variant(unsigned number) : type(2), number(number) {}
    explicit Variant(InputStream &is) : type(0), number(0) {
        is | type;
        if (type==2)
            is | number;
    }
    bool isBoolean() const {
        return type<2;
    }
    bool getBoolean() const {
        if (!isBoolean())
            throw "type mismatch";
        else
            return type==1;
    }
    bool isNumber() const {
        return type==2;
    }
    unsigned getNumber() const {
        if (!isNumber())
            throw "type mismatch";
        else
            return number;
    }
    void serialize(OutputStream &os) const {
        os | type;
        if (isNumber())
            os | number;
    }
    bool operator ==(const Variant &other) const {
        if (type!=other.type)
            return false;
        else if ((type==2)&&(number!=other.number))
            return false;
        else
            return true;
    }
    bool operator !=(const Variant &other) const {
        return !operator ==(other);
    }
    
private:
    unsigned char type;
    unsigned number;
};

static int sequence(size_t index) {
    return (index*539871334)%2395983;
}

template <class T>
static void readValue(InputStream &is, T correct, const char * error) {
    T x;
    is | x;
    if (x!=correct)
        throw error;
}

static void readVariant(InputStream &is, Variant correct, const char * error) {
    Variant v(is);
    if (v!=correct)
        throw error;
}

template <class T>
static void readInteger(InputStream &is) {
    readValue<T>(is, 0, "wrong integer");
    readValue<T>(is, T(0xfeac9c0), "wrong integer");
}

template <class T>
static void readFloat(InputStream &is) {
    readValue<T>(is, 0.0, "wrong float");
    readValue<T>(is, M_PI, "wrong float");
}

template <class T>
static void writeInteger(OutputStream &stream) {
    stream | T(0) | T(0xfeac9c0);
}

template <class T>
static void writeFloat(OutputStream &stream) {
    stream | T(0.0) | T(M_PI);
}

int main(int argc, char ** argv) {
    (void)argc;
    
    static const char * FILENAME="temporary.data";
    static const uint8_t MAGIC[8]={'R', 'o', 'h', 'a', 'n', '.', '~', '1'};
    const size_t ARRAYSIZE=8;
    const size_t NFIB=64;
    
    try {
        // Various arrays of integers
        int ints[ARRAYSIZE];
        std::array<int, ARRAYSIZE> integers;
        std::vector<int> integerv(ARRAYSIZE);
        std::vector<std::pair<unsigned, float>> pairs(ARRAYSIZE);
        std::map<unsigned, std::string> map;
        std::vector<Variant> variantv;
        for (unsigned i=0; i<ARRAYSIZE; i++) {
            ints[i]=integers[i]=integerv[i]=sequence(i);
            pairs[i].first=i;
            pairs[i].second=sequence(i);
            char buffer[32];
            sprintf(buffer, "{%u==0x%08x}", i, i);
            map[i]=buffer;
            if (i%9==0)
                variantv.emplace_back(true);
            else if (i%2==0)
                variantv.emplace_back(false);
            else
                variantv.emplace_back(i);
        }
        
        // C-style strings
        const char * cstring="cstring";
        const wchar_t * wcstring=L"wcstring";
        
        // C++-style strings
        std::string cxxstring(cstring);
        std::wstring wcxxstring(wcstring);
        
        {
            File of(FILENAME, O_WRONLY|O_CREAT|O_TRUNC, 0600);
            FileOutputStream fos(of.getDescriptor());
            
            // Write magic number and version
            fos | MAGIC | uint8_t(1);
            
            // Write first Fibonacci numbers
            fos | NFIB;
            uint64_t a=0, b=1;
            for (unsigned i=0; i<NFIB; i++) {
                fos | a;
                uint64_t tmp=b;
                b+=a;
                a=tmp;
            }
            
            // Write boolean values
            bool trueValue=true, falseValue=false;
            fos | trueValue | falseValue;
            
            // Write integers
            writeInteger<char>(fos);
            writeInteger<signed char>(fos);
            writeInteger<unsigned char>(fos);
            writeInteger<short>(fos);
            writeInteger<signed short>(fos);
            writeInteger<unsigned short>(fos);
            writeInteger<int>(fos);
            writeInteger<signed int>(fos);
            writeInteger<unsigned int>(fos);
            writeInteger<long>(fos);
            writeInteger<signed long>(fos);
            writeInteger<unsigned long>(fos);
            writeInteger<long long>(fos);
            writeInteger<signed long long>(fos);
            writeInteger<unsigned long long>(fos);
            
            // Write integers with stdint-style names
            writeInteger<int8_t>(fos);
            writeInteger<uint8_t>(fos);
            writeInteger<int16_t>(fos);
            writeInteger<uint16_t>(fos);
            writeInteger<int32_t>(fos);
            writeInteger<uint32_t>(fos);
            writeInteger<int64_t>(fos);
            writeInteger<uint64_t>(fos);
            
            // Write floating-point values
            writeFloat<float>(fos);
            writeFloat<double>(fos);
            writeFloat<long double>(fos);
            
            // Write various arrays of integers
            fos | ints | integers | integerv;
            
            // Write strings
            fos | cstring | wcstring | cxxstring | wcxxstring;
            
            // Write pairs
            fos | pairs;
            
            // Write maps
            fos | map;
            
            // Write single variant values
            fos | Variant(false) | Variant(true) | Variant(0x31337U);
            
            // Write an array of variants
            fos | variantv;
        }
        
        {
            File file(FILENAME);
            FileInputStream fis(file.getDescriptor());
            
            // Read magic number and version
            uint8_t magic[8], version;
            fis | magic | version;
            if (0!=memcmp(MAGIC, magic, 8))
                throw "wrong magic number";
            if (version!=1)
                throw "wrong version";
            
            // Read first Fibbonaci numbers
            unsigned count;
            fis | count;
            uint64_t a=0, b=1;
            for (unsigned i=0; i<count; i++) {
                readValue<uint64_t>(fis, a, "wrong Fibonacci number");
                uint64_t tmp=b;
                b+=a;
                a=tmp;
            }
            
            // Read boolean values
            readValue<bool>(fis, true, "wrong boolean");
            readValue<bool>(fis, false, "wrong boolean");
            
            // Read integers
            readInteger<char>(fis);
            readInteger<signed char>(fis);
            readInteger<unsigned char>(fis);
            readInteger<short>(fis);
            readInteger<signed short>(fis);
            readInteger<unsigned short>(fis);
            readInteger<int>(fis);
            readInteger<signed int>(fis);
            readInteger<unsigned int>(fis);
            readInteger<long>(fis);
            readInteger<signed long>(fis);
            readInteger<unsigned long>(fis);
            readInteger<long long>(fis);
            readInteger<signed long long>(fis);
            readInteger<unsigned long long>(fis);
            
            // Read integers with stdint-style names
            readInteger<int8_t>(fis);
            readInteger<uint8_t>(fis);
            readInteger<int16_t>(fis);
            readInteger<uint16_t>(fis);
            readInteger<int32_t>(fis);
            readInteger<uint32_t>(fis);
            readInteger<int64_t>(fis);
            readInteger<uint64_t>(fis);
            
            // Read floating-point values
            readFloat<float>(fis);
            readFloat<double>(fis);
            readFloat<long double>(fis);
            
            // Read various arrays of integers
            int nints[ARRAYSIZE];
            std::array<int, ARRAYSIZE> nintegers;
            std::vector<int> nintegerv;
            fis | nints | nintegers | nintegerv;
            if (integerv.size()!=nintegerv.size())
                throw "wrong size of vector";
            for (size_t i=0; i<ARRAYSIZE; i++) {
                if (ints[i]!=nints[i])
                    throw "wrong C-style array of integers";
                if (integers[i]!=nintegers[i])
                    throw "wrong C++-style array of integers";
                if (integerv[i]!=nintegerv[i])
                    throw "wrong array of integers";
            }
            
            // Read C++-style strings
            readValue<std::string>(fis, cstring, "wrong string");
            readValue<std::wstring>(fis, wcstring, "wrong wstring");
            readValue<std::string>(fis, cxxstring, "wrong string");
            readValue<std::wstring>(fis, wcxxstring, "wrong wstring");
            
            // Read pairs
            readValue(fis, pairs, "wrong pairs");
            
            // Read map
            readValue(fis, map, "wrong map");
            
            // Read single variant values
            readVariant(fis, Variant(false), "wrong variant #1");
            readVariant(fis, Variant(true), "wrong variant #2");
            readVariant(fis, Variant(0x31337U), "wrong variant #3");
            
            // Read an array of variants
            std::vector<Variant> nvariantv;
            fis | nvariantv;
            if (variantv.size()!=nvariantv.size())
                throw "wrong size of variant vector";
            for (size_t i=0; i<variantv.size(); i++)
                if (variantv[i]!=nvariantv[i])
                    throw "wrong variant";
        }
        
        cerr << "SUCCESS" << endl;
        
        return 0;
    }
    catch (const char * error) {
        cerr << argv[0] << ": " << error << endl;
        return 1;
    }
    catch (int error) {
        cerr << argv[0] << ": " << strerror(error) << endl;
        return 1;
    }
}
