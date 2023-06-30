#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <openssl/rand.h>
#include "cryptor.h"
#include "errorcodes.h"

Cryptor::Cryptor(std::filesystem::path path)
{
    pathString = path.string();
}

Cryptor::Cryptor(std::string path)
{
    pathString = path;
}

Hasher::Hasher(HashFunctionType hft)
{
    hashFunctionType = hft;
}

void Hasher::setSalt(std::string rawsalt)
{
    salt = rawsalt;
}

void Hasher::generateSalt(unsigned int length)
{
    auto buf = new unsigned char [length];
    if (RAND_bytes(buf, length) != 1)
    {
        delete [] buf;
        std::cerr << "Error: Cannot generate salt" << std::endl;
        exit(CANNOT_GENERATE_SALT);
    }
    // Successfully filled with random bytes
    salt = std::string { (char*) buf, length };
    delete [] buf;
}
