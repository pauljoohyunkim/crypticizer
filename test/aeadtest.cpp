#include "../src/cryptor.h"
#include <iostream>

int main()
{
    LogCryptor logcryptor { std::string("password"), std::string("NaCl") };
    logcryptor.setLog(Log(std::string("."), (time_t) 1));

    logcryptor.createTempFile();

    std::cout << "Make sure there is a text file at the newly created temp file path" << std::endl;
    std::cout << "Press enter to continue";
    std::cin.ignore();

    logcryptor.generateIV();

    logcryptor.encrypt();

    logcryptor.decrypt();

    logcryptor.cleanupTempFile();

    return 0;
}
