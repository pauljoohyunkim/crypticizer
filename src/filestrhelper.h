#ifndef FILESTRHELPER_H
#define FILESTRHELPER_H

/*
 * A few functions for working with reading and writing easier.
 */

#include <string>
#include <filesystem>

std::string readFileToString(std::filesystem::path path);
std::string readFileToString(std::string pathstr);
std::vector<std::string> readFileToStrLines(std::filesystem::path pathstr);
std::vector<std::string> readFileToStrLines(std::string pathstr);

#endif

