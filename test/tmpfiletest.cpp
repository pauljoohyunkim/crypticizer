#include "../src/session.h"
#include "../src/cryptor.h"

int main()
{
    std::string inputFileName { "test.txt.encrypted" };
    std::string password { "password" };
    std::string salt { "NaCl" };
    LogCryptor logcryptor { password, salt };
    logcryptor.decrypt(inputFileName);

    logcryptor.cleanupTempFile();

    return 0;
}
