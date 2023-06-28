#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <filesystem>
#include <vector>

class Session;
class Log;

class Session
{
    public:
        // Setter
        void setSessionPath(std::filesystem::path path);

        // Getter
        std::filesystem::path getSessionPath();

        // Add entryFilePath
        void addLog(std::filesystem::path p);
        //void addLog(std::filesystem::path p, std::time_t timer);
    private:
        // Ordering
        void orderLogs();

        std::filesystem::path sessionPath {};
        std::vector<Log> logs {};

};

class Log
{
    public:
        Log(std::filesystem::path alogpath);
        Log(std::filesystem::path alogpath, std::time_t atimer);

        std::filesystem::path logpath {};
    private:
        std::time_t timer;
};

#endif
