#include "../src/cryptor.h"

int main()
{
    LogCryptor logcryptor { std::string("password"), std::string("NaCl") };

    logcryptor.generateIV(10);

    logcryptor.encrypt();

    return 0;
}
