#include "../src/cryptor.h"

int main()
{
    LogCryptor logcryptor { std::string("test.txt") };

    logcryptor.generateIV(10);

    logcryptor.encrypt();

    return 0;
}