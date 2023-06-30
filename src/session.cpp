#include <string>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include "session.h"

static bool logCompare(Log log1, Log log2);

void Session::setSessionPath(std::filesystem::path path)
{
    sessionPath = path;
}

void Session::setSessionPassword(std::string pass)
{
    password = pass;
}

std::filesystem::path Session::getSessionPath()
{
    return sessionPath;
}

std::string Session::getSessionPassword()
{
    return password;
}

std::vector<Log> Session::getLogs()
{
    return logs;
}

void Session::addLog(std::filesystem::path p)
{
    Log log { p };
    logs.push_back(log);
    orderLogs();
}
void Session::addLog(std::filesystem::path p, std::time_t timer)
{
    Log log { p, timer };
    logs.push_back(log);
    orderLogs();
}
void Session::clearLog()
{
    logs.clear();
}

void Session::orderLogs()
{
    std::sort(logs.begin(), logs.end(), logCompare);
}

// For ordering of paths
static bool logCompare(Log log1, Log log2)
{
    auto fn1 { log1.logpath.stem().string() };
    auto fn2 { log2.logpath.stem().string() };
    return std::stoi(fn1) < std::stoi(fn2);
}

Log::Log(std::filesystem::path alogpath)
{
    logpath = alogpath;
    timer = time(nullptr);
}
Log::Log(std::filesystem::path alogpath, std::time_t atimer)
{
    logpath = alogpath;
    timer = atimer;
}
