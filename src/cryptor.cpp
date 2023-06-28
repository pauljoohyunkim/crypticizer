#include <fstream>
#include <string>
#include <filesystem>
#include "cryptor.h"

Cryptor::Cryptor(std::filesystem::path path)
{
    pathString = path.string();
}

Cryptor::Cryptor(std::string path)
{
    pathString = path;
}
