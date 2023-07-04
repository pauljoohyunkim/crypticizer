#include "../src/cryptor.h"

int main()
{
    LogCryptor logcryptor { std::string("password"), std::string("NaCl") };
    logcryptor.setLog(Log(std::string("test.txt.encrypted")));

    logcryptor.generateIV();

    logcryptor.encrypt(std::string("test.txt"), std::string("test.txt.encrypted"));

    logcryptor.decrypt();

    logcryptor.cleanupTempFile();

    return 0;
}
