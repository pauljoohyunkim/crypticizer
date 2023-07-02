#include <iostream>
#include <string>
#include "../src/cryptor.h"

int main()
{
    std::string pass { "password" };
    std::string salt { "NaCl" };
    auto expandedKey = scryptKDF(pass, 64, salt);

    return 0;
}
