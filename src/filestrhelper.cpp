#include <fstream>
#include <filesystem>
#include <string>
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

    auto filecontent = new char [filelen];

    // Read
    infile.read(filecontent, filelen);
    infile.close();

    // Turn it into string
    std::string contentString { filecontent, filecontent + filelen };
    delete [] filecontent;

    return contentString;
}
