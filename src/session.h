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
        void setSessionTextEditor(std::string texteditor);

        // Getter
        std::filesystem::path getSessionPath();
        std::string getSessionPassword();
        std::string getSessionTextEditor();
        std::vector<Log> getLogs();

        // Add Log
        void addLog();
        void addLog(std::time_t timer);

        // Clear Log
        void clearLog();

    private:
        // Ordering
        void orderLogs();

        std::filesystem::path sessionPath {};
        std::vector<Log> logs {};
        std::string password {};
        std::string textEditorName { "vim" };

};

class Log
{
    public:
        Log();
        Log(std::filesystem::path rootpath);
        Log(std::filesystem::path rootpath, std::time_t atimer);

        void refreshTime();
        void generateLogPathFromTimer();

        std::string getLocalTime();

        std::filesystem::path logpath {};
    private:
        std::time_t timer;
};

#endif
