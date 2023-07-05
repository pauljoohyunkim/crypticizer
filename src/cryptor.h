#ifndef CRYPTOR_H
#define CRYPTOR_H

#include <fstream>
#include <string>
#include <filesystem>
#include <openssl/evp.h>
#include "session.h"

#define CRYPTOR_IV_LEN 12
#define CRYPTOR_TAG_LEN 16
#define CRYPTOR_SALT_LEN 32

class LogCryptor
{
    public:
        LogCryptor(std::string pass);

        // Generate salt (Reuses the salt generator from Hasher)
        void generateSalt(unsigned int saltByteLen=CRYPTOR_SALT_LEN);

        // Set Log
        void setLog(Log alog);

        std::string generateIV(unsigned int byteLength=CRYPTOR_IV_LEN);
        // Takes a temp file in tempdir, encrypts, then removes the temp file.
        void encrypt();
        // If preview=false, it takes an encrypted file, decrypts it to tempdir, returns the path string to tempdir (if preview=false)
        // If preview=true, then it decrypts the file in memory, and it instead returns the decrypted text.
        std::string decrypt(bool preview=false, unsigned int saltnLen=CRYPTOR_SALT_LEN, unsigned int ivLen=CRYPTOR_IV_LEN, unsigned int tagLen=CRYPTOR_TAG_LEN);
        
        // Creates temp file at /tmp/crypticizer.XXXXXX then returns the path to it
        std::string createTempFile();

        // Cleans up the temp file.
        void cleanupTempFile();
    private:
        std::string iv {};
        std::string password {};
        std::string salt {};
        Log log;
        std::FILE* tempfileHandle;
        bool tempfileHandleClosed { true };
        std::string currentTEMPFilePath {};

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
