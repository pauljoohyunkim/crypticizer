#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "filestrhelper.h"


std::string readFileToString(std::filesystem::path path)
{
    return readFileToString(path.string());
}

std::string readFileToString(std::string pathstr)
{
    std::ifstream infile { pathstr };

    // Determine file length.
    infile.seekg(0, infile.end);
    unsigned int filelen = infile.tellg();
    infile.seekg(0, infile.beg);

    auto filecontent_smart { std::make_unique<char[]>(filelen)};
    auto filecontent { filecontent_smart.get() };

    // Read
    infile.read(filecontent, filelen);
    infile.close();

    // Turn it into string
    std::string contentString { filecontent, filecontent + filelen };

    return contentString;
}

std::vector<std::string> readFileToStrLines(std::filesystem::path pathstr)
{
    return readFileToStrLines(pathstr.string());
}

std::vector<std::string> readFileToStrLines(std::string pathstr)
{
    std::vector<std::string> lines {};
    std::ifstream infile { pathstr };

    while (!infile.eof())
    {
        std::string strline {};
        std::getline(infile, strline);
        lines.push_back(strline);
    }

    infile.close();

    return lines;
}
