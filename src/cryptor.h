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

        // Set Digest
        void setDigest(std::string rawdigest);
        // Digest
        std::string digestWithSalt(std::string message);

        // Output hexdigest (requires salt and digest to be assigned)
        std::string hexdigest();
        void dumpHexdigestToFile(std::filesystem::path path);

        void readHexdigestFile(std::filesystem::path path, unsigned int saltByteLen);
        
        // Verify hash
        // Need readHexdigestFile to be called beforehand to make referenceHexdigest to be filled.
        bool verifyHash();

    private:
        HashFunctionType hashFunctionType { HFT_SHA512 };
        std::string hashFunctionName {};
        unsigned int digestLength;
        std::string salt {};
        std::string digest {};
        std::string referenceHexdigest {};
};

#endif
