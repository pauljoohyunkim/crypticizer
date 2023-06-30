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

enum HashFunctionType
{
    HFT_SCRYPT = 0,
};

class Hasher
{
    public:
        Hasher(HashFunctionType hft);
        
        // Set Salt
        void setSalt(std::string rawsalt);

        void generateSalt(unsigned int length);

    private:
        HashFunctionType hashFunctionType { HFT_SCRYPT };
        std::string salt {};
        std::string digest {};
};

#endif
