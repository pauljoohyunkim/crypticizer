#ifndef SESSION_H
#define SESSION_H

#include <ctime>
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
        void setSessionPassword(std::string pass);

        // Getter
        std::filesystem::path getSessionPath();
        std::string getSessionPassword();
        std::vector<Log> getLogs();

        // Add Log
        void addLog(std::filesystem::path p);
        void addLog(std::filesystem::path p, std::time_t timer);

        // Clear Log
        void clearLog();

    private:
        // Ordering
        void orderLogs();

        std::filesystem::path sessionPath {};
        std::vector<Log> logs {};
        std::string password {};

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
