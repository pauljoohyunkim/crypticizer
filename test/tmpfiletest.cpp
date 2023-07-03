#include "../src/session.h"
#include "../src/cryptor.h"

int main()
{
    LogCryptor logcryptor { std::string("test.txt"), std::string("salt") };
    auto filehandle = logcryptor.createTempFile();

    std::fclose(filehandle);

    return 0;
}
