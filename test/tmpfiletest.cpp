#include "../src/session.h"
#include "../src/cryptor.h"
#include <filesystem>

int main()
{
    Log log { std::filesystem::path("test.txt.encrypted") };
    std::string password { "password" };
    std::string salt { "NaCl" };
    LogCryptor logcryptor { password, salt };
    logcryptor.setLog(log);
    logcryptor.decrypt();

    logcryptor.cleanupTempFile();

    return 0;
}
