#include <string>
#include <filesystem>
#include "session.h"

void Session::setSessionPath(std::filesystem::path path)
{
    sessionPath = path;
}

std::filesystem::path Session::getSessionPath()
{
    return sessionPath;
}