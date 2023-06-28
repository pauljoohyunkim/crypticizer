#include <string>
#include <filesystem>
#include <algorithm>
#include "session.h"

static bool entryPathCompare(std::filesystem::path p1, std::filesystem::path p2);

void Session::setSessionPath(std::filesystem::path path)
{
    sessionPath = path;
}

std::filesystem::path Session::getSessionPath()
{
    return sessionPath;
}

void Session::addEntryFilePath(std::filesystem::path p)
{
    entryFilePaths.push_back(p);
    orderEntryFilePaths();
}

void Session::orderEntryFilePaths()
{
    std::sort(entryFilePaths.begin(), entryFilePaths.end());
}

// For ordering of paths
static bool operator<(std::filesystem::path p1, std::filesystem::path p2)
{
    auto fn1 { p1.stem() };
    auto fn2 { p2.stem() };
    return std::stoi(fn1) < std::stoi(fn2);
}
