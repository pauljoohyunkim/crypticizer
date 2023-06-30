#include "../src/cryptor.h"
#include <iostream>

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

    std::cout << hasher.hexdigest() << std::endl;
    return 0;
}
