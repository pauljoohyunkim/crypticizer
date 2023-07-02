#include "../src/cryptor.h"

int main()
{
    LogCryptor logcryptor { std::string("password"), std::string("NaCl") };

    logcryptor.generateIV(12);

    logcryptor.encrypt(std::string("test.txt"), std::string("test.txt.encrpted"));

    return 0;
}
