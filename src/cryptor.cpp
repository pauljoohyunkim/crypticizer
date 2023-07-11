#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <tuple>
#include <regex>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <openssl/rand.h>
#include <openssl/kdf.h>
#include <openssl/evp.h>
#include <ncurses.h>
#include "cryptor.h"
#include "session.h"
#include "errorcodes.h"

static std::string hexToRaw(std::string hexstring);

static const std::unordered_map<HashFunctionType, std::pair<std::string, unsigned int>> hftToMdName = 
{
    { HFT_SHA256, { "sha256", 256 } },
    { HFT_SHA512, { "sha512", 512 } }
};

LogCryptor::LogCryptor(std::string pass)
{
    password = pass;
}

void LogCryptor::generateSalt(unsigned int saltByteLen)
{
    // Reusing the salt generator from Hasher
    Hasher hasher { HFT_SHA512 };
    hasher.generateSalt(saltByteLen);
    salt = hasher.getSalt();
}

void LogCryptor::setLog(Log alog)
{
    log = alog;
}

std::string LogCryptor::generateIV(unsigned int byteLength)
{
    auto buf = new unsigned char[byteLength];

    RAND_bytes(buf, byteLength);
    std::string ivString { buf, buf + byteLength };
    delete [] buf;

    iv = ivString;
    return iv;
}

// https://crypto.stackexchange.com/questions/26783/ciphertext-and-tag-size-and-iv-transmission-with-aes-in-gcm-mode
void LogCryptor::encrypt()
{
    // Getting file info
    auto infilename = currentTEMPFilePath;
    std::ifstream inFile { infilename };
    inFile.seekg(0, inFile.end);
    unsigned int filelength { static_cast<unsigned int>(inFile.tellg()) };
    inFile.seekg(0, inFile.beg);
    auto plaintext = new char [filelength];
    auto plaintext_len = filelength;
    // Read plaintext
    inFile.read(plaintext, plaintext_len);
    // Generate salt if not already generated
    if (salt.empty())
    {
        generateSalt();
    }
    // Generate IV if not already generated
    if (iv.empty())
    {
        generateIV();
    }

    auto expandedKey { scryptKDF(password, (int) 256 / 8, salt) };
    auto ciphertext = new unsigned char [plaintext_len];
    
    EVP_CIPHER_CTX* ctx;
    int len;
    int ciphertext_len;

    if(!(ctx = EVP_CIPHER_CTX_new()))
    {
        std::cerr<< "EVP_CIPHER_CTX_new()" << std::endl;
    }

    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
    {
        std::cerr << "EVP_EncryptInit_ex()" << std::endl;
    }

    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.length(), NULL))
    {
        std::cerr << "EVP_CIPHER_CTX_ctrl()" << std::endl;
    }

    // Initialization of key and iv
    if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, (unsigned char*) expandedKey.c_str(), (unsigned char*) iv.c_str()))
    {
        std::cerr << "EVP_EncryptInit_ex" << std::endl;
    }
    
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, (unsigned char*) plaintext, plaintext_len))
    {
        std::cerr << "EVP_EncryptUpdate" << std::endl;
    }
    ciphertext_len = len;

    // Finalization
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
    {
        std::cerr << "EVP_EncryptFinal_ex" << std::endl;
    }
    ciphertext_len += len;

    unsigned char tag[CRYPTOR_TAG_LEN];

    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, CRYPTOR_TAG_LEN, tag))
    {
        std::cerr << "EVP_CIPHER_CTX_ctrl" << std::endl;
    }

    EVP_CIPHER_CTX_free(ctx);

    std::string ciphertextString { ciphertext, ciphertext + ciphertext_len };
    std::string tagString { tag, tag + CRYPTOR_TAG_LEN };

    // Export
    auto outfilename = log.logpath.string();
    std::ofstream outFile { outfilename, std::ofstream::binary};
    // Salt + IV + CIPHERTEXT + Tag (16 bytes)
    outFile << salt << iv << ciphertextString << tagString;

    delete [] plaintext;
    delete [] ciphertext;
    cleanupTempFile();
}

std::string LogCryptor::decrypt(bool preview, unsigned int saltLen, unsigned int ivLen, unsigned int tagLen)
{
    auto infilename { log.logpath.string() };
    // Input file info (Note: Salt + IV + Ciphertext + Tag (16 bytes)
    std::ifstream inFile { infilename, std::ifstream::binary };
    inFile.seekg(0, inFile.end);
    unsigned int filelength { static_cast<unsigned int>(inFile.tellg()) };
    inFile.seekg(0, inFile.beg);
    auto infilecontent = new char [filelength];
    auto infilecontent_len = filelength;
    // Read encrypted file
    inFile.read(infilecontent, infilecontent_len);

    // Ciphertext length
    unsigned int ciphertext_len = infilecontent_len - ivLen - tagLen - saltLen;

    // Set IV, Ciphertext, Tag
    salt = std::string { infilecontent, infilecontent + saltLen };
    iv = std::string { infilecontent + saltLen, infilecontent + saltLen + ivLen };
    std::string ciphertext { infilecontent + saltLen + ivLen, infilecontent + saltLen + ivLen + ciphertext_len };
    std::string tag { infilecontent + saltLen + ivLen + ciphertext_len, infilecontent + filelength };
    delete [] infilecontent;

    auto expandedKey { scryptKDF(password, (int) 256 / 8, salt) };

    auto plaintext = new unsigned char [ciphertext_len];
    int plaintext_len;
    int len;
    int ret;

    EVP_CIPHER_CTX* ctx;

    if(!(ctx = EVP_CIPHER_CTX_new()))
    {
        std::cerr<< "EVP_CIPHER_CTX_new()" << std::endl;
    }

    if(!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
    {
        std::cerr << "EVP_DecryptInit_ex()" << std::endl;
    }

    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.length(), NULL))
    {
        std::cerr << "EVP_CIPHER_CTX_ctrl()" << std::endl;
    }

    // Initialization of key and iv
    if (!EVP_DecryptInit_ex(ctx, NULL, NULL, (unsigned char*) expandedKey.c_str(), (unsigned char*) iv.c_str()))
    {
        std::cerr << "EVP_DecryptInit_ex" << std::endl;
    }

    if (!EVP_DecryptUpdate(ctx, plaintext, &len, (unsigned char*) ciphertext.c_str(), ciphertext_len))
    {
        std::cerr << "EVP_EncryptUpdate" << std::endl;
    }
    plaintext_len = len;

    if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tagLen, (unsigned char*) tag.c_str()))
    {
        std::cerr << "EVP_CIPHER_CTX_ctrl" << std::endl;
    }

    // Return value
    ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    if(ret > 0) {
        /* Success */
        plaintext_len += len;
    } else {
        /* Verify failed */
        def_prog_mode();
        endwin();
        std::cerr << "Verification failed, not trustworthy" << std::endl;
        std::cin.ignore();
        reset_prog_mode();
        refresh();
    }
    std::string plaintextString { plaintext, plaintext + plaintext_len };
    delete [] plaintext;

    // Create temp file with the decrypted text.
    if (!preview)
    {
        // Output file
        auto tempString = createTempFile();
        fwrite(plaintextString.c_str(), plaintextString.length(), 1, tempfileHandle);
        fclose(tempfileHandle);
        tempfileHandleClosed = true;

        return tempString;
    }
    // Otherwise return the plaintext content.
    return plaintextString;
}

std::string LogCryptor::createTempFile()
{
    auto tmpdirPath { std::filesystem::temp_directory_path() };
    auto tmpfilePath { tmpdirPath/std::string("crypticizer.XXXXXX") };
    auto tmpfilePathLength { tmpfilePath.string().length() };
    auto tmpfilename = new char [tmpfilePathLength + 1];
    tmpfilename[tmpfilePathLength] = '\0';
    strcpy(tmpfilename, tmpfilePath.string().c_str());

    // Create temp file
    auto fd = mkstemp(tmpfilename);
    tempfileHandle = fdopen(fd, "w");
    currentTEMPFilePath = std::string(tmpfilename);
    delete [] tmpfilename;

    tempfileHandleClosed = false;
    return currentTEMPFilePath;
}

void LogCryptor::cleanupTempFile()
{
    // Close file
    if (!tempfileHandleClosed)
    {
        std::fclose(tempfileHandle);
    }
    tempfileHandleClosed = true;


    // Clean up
    unlink(currentTEMPFilePath.c_str());

    // Empty the temp file path
    currentTEMPFilePath.clear();
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

    std::string hexDigestString { buf, buf+inFile.gcount() };
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

std::string scryptKDF(std::string key, unsigned int keyExpandedLength, std::string salt)
{
    EVP_KDF* kdf;
    EVP_KDF_CTX* kctx;

    auto buf = new unsigned char [keyExpandedLength];
    OSSL_PARAM params[6];

    kdf = EVP_KDF_fetch(NULL, "SCRYPT", NULL);
    kctx = EVP_KDF_CTX_new(kdf);
    EVP_KDF_free(kdf);

    uint64_t n = 1024;
    uint32_t r = 8;
    uint32_t p = 16;
    params[0] = OSSL_PARAM_construct_octet_string("pass",
            (void*)key.c_str(), (size_t)key.length());
    params[1] = OSSL_PARAM_construct_octet_string("salt",
            (void*)salt.c_str(), (size_t)salt.length());
    params[2] = OSSL_PARAM_construct_uint64("n", &n);
    params[3] = OSSL_PARAM_construct_uint32("r", &r);
    params[4] = OSSL_PARAM_construct_uint32("p", &p);
    params[5] = OSSL_PARAM_construct_end();

    if (EVP_KDF_derive(kctx, buf, keyExpandedLength, params) <= 0)
    {
        delete [] buf;
        std::cerr << "Error: Could not compute Scrypt key derivation function" << std::endl;
        exit(SCRYPT_ERROR);
    }
    std::string expandedKey { buf, buf+keyExpandedLength };
    delete [] buf;
    EVP_KDF_CTX_free(kctx);
	return expandedKey;
}
