#ifndef CRYPTOR_H
#define CRYPTOR_H

#include <fstream>
#include <string>
#include <filesystem>
#include <openssl/evp.h>

class Cryptor
{
    public:
        Cryptor(std::filesystem::path path);
        Cryptor(std::string path);
    private:
        std::string pathString {};

};

// Enum value inspired from Linux shadow file hash function type
enum HashFunctionType
{
    HFT_SHA256 = 5,
    HFT_SHA512 = 6
};

class Hasher
{
    public:
        Hasher(HashFunctionType hft);
        
        // Set Salt
        void setSalt(std::string rawsalt);

        void generateSalt(unsigned int length);

        void digestWithSalt(std::string message);

    private:
        HashFunctionType hashFunctionType { HFT_SHA512 };
        std::string hashFunctionName {};
        unsigned int digestLength;
        std::string salt {};
        std::string digest {};
};

#endif
