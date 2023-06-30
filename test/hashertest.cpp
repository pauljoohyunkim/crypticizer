#include "../src/cryptor.h"

int main()
{
    Hasher hasher { HFT_SHA512 };

    // Setting salt test
    std::string givenSalt { "hello" };
    hasher.setSalt(givenSalt);

    // Generating salt test
    hasher.generateSalt(10);

    // Clear salt
    hasher.setSalt(std::string());

    // Test Message
    std::string mess { "Test Message" };
    hasher.digestWithSalt(mess);
    return 0;
}
