#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <tuple>
#include <openssl/rand.h>
#include "cryptor.h"
#include "errorcodes.h"

static const std::unordered_map<HashFunctionType, std::pair<std::string, unsigned int>> hftToMdName = 
{
    { HFT_SHA256, { "sha256", 256 } },
    { HFT_SHA512, { "sha512", 512 } }
};

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
    auto hashFunctionDetails = hftToMdName.at(hft);
    hashFunctionName = hashFunctionDetails.first;
    digestLength = hashFunctionDetails.second;
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

void Hasher::digestWithSalt(std::string message)
{
    EVP_MD_CTX* mdctx;
    EVP_MD* md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int digestByteLength = digestLength / 8;

    md = EVP_MD_fetch(NULL, hashFunctionName.c_str(), NULL);
    mdctx = EVP_MD_CTX_new();

    // Initialize
    if(!EVP_DigestInit_ex2(mdctx, md, NULL))
    {
        std::cerr << "Error: Cannot initialize hasher digester" << std::endl;
        EVP_MD_CTX_free(mdctx);
        exit(CANNOT_INITIALIZE_DIGEST);
    }

    // Salt Digest
    if(!EVP_DigestUpdate(mdctx, salt.c_str(), salt.length()))
    {
        std::cerr << "Error: Cannot update hasher digester" << std::endl;
        EVP_MD_CTX_free(mdctx);
        exit(CANNOT_UPDATE_DIGEST);
    }
    // Message Digest
    if(!EVP_DigestUpdate(mdctx, message.c_str(), message.length()))
    {
        std::cerr << "Error: Cannot update hasher digester" << std::endl;
        EVP_MD_CTX_free(mdctx);
        exit(CANNOT_UPDATE_DIGEST);
    }

    // Finalize
    if(!EVP_DigestFinal_ex(mdctx, md_value, NULL))
    {
        std::cerr << "Error: Cannot finalize hasher digester" << std::endl;
        EVP_MD_CTX_free(mdctx);
        exit(CANNOT_FINALIZE_DIGEST);
    }
    EVP_MD_CTX_free(mdctx);

    digest = std::string(md_value, md_value+digestByteLength);
}

std::string Hasher::hexdigest()
{
    std::stringstream ss;
    for (unsigned char c : salt)
    {
        ss << std::setfill('0') << std::setw(2) << std::hex << (unsigned int) c;
    }
    ss << '$';
    for (unsigned char c : digest)
    {
        ss << std::setfill('0') << std::setw(2) << std::hex << (unsigned int) c;
    }

    return ss.str();
}
