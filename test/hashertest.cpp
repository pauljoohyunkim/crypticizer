#include "../src/cryptor.h"
#include <iostream>
#include <filesystem>

int main()
{
    Hasher hasher { HFT_SHA512 };
    Hasher hasher2 { HFT_SHA512 };

    // Setting salt test
    std::string givenSalt { "hello" };
    hasher.setSalt(givenSalt);

    // Generating salt test
    hasher.generateSalt(10);
    // Test Message
    std::string mess { "Test Message" };
    auto digest = hasher.digestWithSalt(mess);

    auto hexdigest = hasher.hexsaltdigest();
    std::cout << "Hasher1 Hexdigest: " << hexdigest << std::endl;

    //hasher2.setSalt(std::string());
    //hasher2.setDigest(digest);
    //std::cout << "Hasher2 Hexdigest: " << hasher2.hexdigest() << std::endl;

    std::filesystem::path path { "./test.txt" };    
    hasher.dumpHexdigestToFile(path);

    auto hasher3 = readHexdigestFile(path, HFT_SHA512, 10);

    // Retrieval
    //hasher.readHexdigestFile(path, 10);
    
    // Verify
    //auto verifyFlag = hasher.verifyHash();
    return 0;
}
