#ifndef CRYPTOR_H
#define CRYPTOR_H

#include <fstream>
#include <string>
#include <filesystem>

class Cryptor
{
    public:
        Cryptor(std::filesystem::path path);
        Cryptor(std::string path);
    private:
        std::string pathString {};

};

#endif
