#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <filesystem>
#include <vector>

class Session
{
    public:
        // Setter
        void setSessionPath(std::filesystem::path path);

        // Getter
        std::filesystem::path getSessionPath();
    private:
        std::filesystem::path sessionPath {};
        std::vector<std::string> logfilenames {};

};

#endif
