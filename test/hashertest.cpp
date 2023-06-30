#include "../src/cryptor.h"

int main()
{
    Hasher hasher { HFT_SCRYPT };

    // Setting salt test
    std::string givenSalt { "hello" };
    hasher.setSalt(givenSalt);

    // Generating salt test
    hasher.generateSalt(10);
    return 0;
}
