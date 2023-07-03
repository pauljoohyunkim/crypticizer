#include "../src/session.h"
#include "../src/cryptor.h"

int main()
{
    LogCryptor logcryptor { std::string("test.txt"), std::string("salt") };
    auto filehandle = logcryptor.createTempFile();

    logcryptor.cleanupTempFile();

    return 0;
}
