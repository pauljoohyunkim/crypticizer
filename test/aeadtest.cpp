#include "../src/cryptor.h"

int main()
{
    LogCryptor logcryptor { std::string("password"), std::string("NaCl") };

    logcryptor.generateIV();

    logcryptor.encrypt(std::string("test.txt"), std::string("test.txt.encrpted"));

    logcryptor.decrypt(std::string("test.txt.encrpted"), std::string("test.txt.decrypted"));

    return 0;
}
