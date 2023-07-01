#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <tuple>
#include <regex>
#include <openssl/rand.h>
#include "cryptor.h"
#include "errorcodes.h"

static std::string hexToRaw(std::string hexstring);

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

void Hasher::setDigest(std::string rawdigest)
{
    digest = rawdigest;
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

std::string Hasher::getSalt()
{
    return salt;
}

std::string Hasher::digestWithSalt(std::string message)
{
    EVP_MD_CTX* mdctx;
    EVP_MD* md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int digestByteLength { (unsigned int) (digestLength / 8) };

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

    return digest;
}

std::string Hasher::hexsaltdigest()
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

void Hasher::dumpHexdigestToFile(std::filesystem::path path)
{
    std::string pathString { path.string() };
    std::ofstream outFile { pathString };
    outFile << hexsaltdigest();
    outFile.close();
}

Hasher readHexdigestFile(std::filesystem::path path, HashFunctionType hft, unsigned int saltByteLen)
{
    Hasher hasher { hft };
    unsigned int digestLength { hftToMdName.at(hft).second };
    unsigned int digestByteLength { (unsigned int) (digestLength / 8) };
    // Reading expected number of bytes only.
    unsigned int readByteLen = 2 * saltByteLen + 1 + 2 * digestByteLength;
    auto buf = new char [readByteLen];
    std::string pathString { path.string() };
    std::ifstream inFile { pathString };

    if (!inFile.read(buf, readByteLen))
    {
        std::cerr << "Error: Unexpected file format" << std::endl;
        delete [] buf;
        inFile.close();
        exit(UNEXPECTED_FILE_FORMAT);
    }

    std::string hexDigestString { buf, buf+readByteLen };
    delete [] buf;

    // Check if form of (salt in hex)$(digest in hex)
    std::stringstream ss;
    ss << "^[0-9a-fA-F]{" << 2 * saltByteLen << "}\\$"
        << "[0-9a-fA-F]{" << 2 * digestByteLength << "}";
    std::regex formChecker { ss.str() };
    std::smatch match;
    if (!std::regex_search(hexDigestString, match, formChecker))
    {
        std::cerr << "Error: Unexpected file format" << std::endl;
        inFile.close();
        exit(UNEXPECTED_FILE_FORMAT);
    }
    inFile.close();

    // hexDigestString is now a valid form.
    // Extract valid information.
    auto hexSalt = hexDigestString.substr(0, 2 * saltByteLen);
    auto hexDigest = hexDigestString.substr(2 * saltByteLen + 1, 2 * digestByteLength);

    auto rawSalt = hexToRaw(hexSalt);
    auto rawDigest = hexToRaw(hexDigest);

    hasher.setSalt(rawSalt);
    hasher.setDigest(rawDigest);

    return hasher;
}

static std::string hexToRaw(std::string hexstring)
{
    std::string rawstring {};
    for (unsigned int i = 0; i < hexstring.length(); i += 2)
    {
        std::string byteString = hexstring.substr(i, 2);
        auto byte = (unsigned char) std::stoi(byteString, nullptr, 16);
        rawstring += byte;
    }

    return rawstring;
}
