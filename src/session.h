#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <filesystem>

class Session
{
    public:
        // Setter
        void setSessionPath(std::filesystem::path path)
        {
            sessionPath = path;
        }
    private:
        std::filesystem::path sessionPath {};
        std::vector<std::string> logfilenames {};

};

#endif
