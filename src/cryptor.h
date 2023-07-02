#ifndef CRYPTOR_H
#define CRYPTOR_H

#include <fstream>
#include <string>
#include <filesystem>
#include <openssl/evp.h>
#include "session.h"

class LogCryptor
{
    public:
        LogCryptor(std::string pass, std::string asalt);

        // Set Log
        void setLog(Log alog);

        std::string generateIV(unsigned int byteLength);
        void encrypt();
    private:
        std::string logPathString {};
        std::string iv {};
        std::string password {};
        std::string salt {};
        Log log;

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

        // Get salt
        std::string getSalt();

        // Set Digest
        void setDigest(std::string rawdigest);
        // Digest
        std::string digestWithSalt(std::string message);

        // Output hexdigest (requires salt and digest to be assigned)
        std::string hexsaltdigest();
        void dumpHexdigestToFile(std::filesystem::path path);

    private:
        HashFunctionType hashFunctionType { HFT_SHA512 };
        std::string hashFunctionName {};
        unsigned int digestLength;
        std::string salt {};
        std::string digest {};
};

Hasher readHexdigestFile(std::filesystem::path path, HashFunctionType hft, unsigned int saltByteLen);
std::string scryptKDF(std::string key, unsigned int keyExpandedLength, std::string salt);

#endif
