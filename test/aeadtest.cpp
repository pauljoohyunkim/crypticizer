#include "../src/cryptor.h"
#include <iostream>

int main()
{
    LogCryptor logcryptor { std::string("password") };
    logcryptor.setLog(Log(std::string("."), (time_t) 1));

    logcryptor.createTempFile();
    //logcryptor.generateIV();

    logcryptor.encrypt();
    //logcryptor.cleanupTempFile();

    logcryptor.decrypt();

    logcryptor.cleanupTempFile();

    return 0;
}
