#ifndef SESSION_H
#define SESSION_H

#include <string>

class Session
{
    public:
        Session(std::string asessionName)
        {
            sessionName = asessionName;
        }
    private:
        std::string sessionName {};

};

#endif
