#include <string>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "session.h"
#include "cryptor.h"

static bool logCompare(Log log1, Log log2);

void Session::setSessionPath(std::filesystem::path path)
{
    sessionPath = path;
}

void Session::setSessionPath(std::filesystem::path sessionpath,
        std::filesystem::path crypticizerpath,
        std::filesystem::path hashfile,
        std::filesystem::path editorfile)
{
    sessionPath = sessionpath;
    crypticizerDirectory = crypticizerpath;
    hashfilepath = hashfile;
    editorfilepath = editorfile;
}

void Session::setSessionPassword(std::string pass)
{
    password = pass;
}

void Session::setSessionTextEditor(std::string texteditor)
{
    textEditorName = texteditor;
}

std::filesystem::path Session::getSessionPath()
{
    return sessionPath;
}

std::filesystem::path Session::getCrypticizerDirectory()
{
    return crypticizerDirectory;
}
std::filesystem::path Session::getHashfilepath()
{
    return hashfilepath;
}
std::filesystem::path Session::getEditorfilepath()
{
    return editorfilepath;
}

std::string Session::getSessionPassword()
{
    return password;
}

std::string Session::getSessionTextEditor()
{
    return textEditorName;
}

std::vector<Log> Session::getLogs()
{
    return logs;
}

void Session::addLog()
{
    Log log { sessionPath };
    logs.push_back(log);
    orderLogs();
}
void Session::addLog(std::time_t timer)
{
    Log log { sessionPath, timer };
    logs.push_back(log);
    orderLogs();
}
void Session::clearLog()
{
    logs.clear();
}
void Session::dumpAsPlaintextFile(std::string pathstr)
{
    std::ofstream outfile { pathstr };

    LogCryptor lc { password };
    for (auto log : logs)
    {
        // Set log, get plaintext, then write to file.
        lc.setLog(log);
        outfile << "======= " << log.getTimer() << " ";
        outfile << log.getLocalTime() << std::endl;
        outfile << lc.decrypt(true);
    }

    outfile.close();
}
void Session::loadPlaintextFile(std::string pathstr)
{
    std::ifstream infile { pathstr };
    while (!infile.eof())
    {
        std::string fileline;
        std::getline(infile, fileline);
    }

    infile.close();
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
    return std::stoi(fn1) > std::stoi(fn2);
}

Log::Log()
{
    refreshTime();
    generateLogPathFromTimer();
}

Log::Log(std::filesystem::path rootpath)
{
    refreshTime();
    generateLogPathFromTimer();
    logpath = rootpath/logpath;
}
Log::Log(std::filesystem::path rootpath, std::time_t atimer)
{
    timer = atimer;
    generateLogPathFromTimer();
    logpath = rootpath/logpath;
}
void Log::refreshTime()
{
    timer = time(nullptr);
}
void Log::generateLogPathFromTimer()
{
    std::stringstream ss;
    ss << timer << ".crpt";
    logpath = std::filesystem::path(ss.str());
}
std::string Log::getLocalTime()
{
    std::stringstream ss;
    auto timeinfo { localtime(&timer) };
    ss << asctime(timeinfo);

    return ss.str();
}
std::time_t Log::getTimer()
{
    return timer;
}
