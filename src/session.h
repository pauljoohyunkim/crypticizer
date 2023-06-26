#ifndef SESSION_H
#define SESSION_H

#include <string>

class Session
{
    public:
        Session(std::string asessionPath)
        {
            sessionPath = asessionPath;
        }
    private:
        std::string sessionPath {};

};

#endif
