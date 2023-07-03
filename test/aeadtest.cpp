#include "../src/cryptor.h"

int main()
{
    LogCryptor logcryptor { std::string("password"), std::string("NaCl") };

    logcryptor.generateIV();

    logcryptor.encrypt(std::string("test.txt"), std::string("test.txt.encrypted"));

    logcryptor.decrypt(std::string("test.txt.encrypted"), std::string("test.txt.decrypted"));

    return 0;
}
