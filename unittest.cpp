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
#include "serialization.hpp"

using namespace rohan;
using namespace std;

class FileStream {
public:
    FileStream(const char * filename, int mode, int rights=0) : fd(open(filename, mode, rights)) {
        if (fd<0)
            throw errno;
    }
    virtual ~FileStream() {
        close(fd);
    }
    
protected:
    FileStream(FileStream &)=delete;
    
    int fd;
};

class FileInputStream : public InputStream, private FileStream {
public:
    FileInputStream(const char * filename) : FileStream(filename, O_RDONLY) {}
    void read(void * data, size_t length) {
        ::read(fd, data, length);
    }
};

class FileOutputStream : public OutputStream, private FileStream {
public:
    FileOutputStream(const char * filename) : FileStream(filename, O_WRONLY|O_TRUNC|O_CREAT, 0600) {}
    void write(const void * data, size_t length) {
        ::write(fd, data, length);
    }
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
        for (unsigned i=0; i<ARRAYSIZE; i++) {
            ints[i]=integers[i]=integerv[i]=sequence(i);
            pairs[i].first=i;
            pairs[i].second=sequence(i);
        }
        
        // C-style strings
        const char * cstring="cstring";
        const wchar_t * wcstring=L"wcstring";
        
        // C++-style strings
        std::string cxxstring(cstring);
        std::wstring wcxxstring(wcstring);
        
        {
            FileOutputStream fos(FILENAME);
            
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
        }
        
        {
            FileInputStream fis(FILENAME);
            
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
