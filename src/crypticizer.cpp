#include <cstdio>
#include <iostream>
#include <filesystem>
#include "crypticizer.h"
#include "session.h"

namespace fs = std::filesystem;

static void detectSession(Session& session);

Session crypticizerSession {};

int main(int argc, char** argv)
{
    /*
     * Usage: crypticizer [session path]
     * where session path is the directory of the project.
     * */
    if (argc > 2)
    {
        std::cout << "Usage: " << argv[0] << " [session path]" << std::endl;
        return EXIT_FAILURE;
    }
    else if (argc == 2)
    {
        
    }
    detectSession(crypticizerSession);

    return EXIT_SUCCESS;
}

// If there exists .crypticizer directory,
static void detectSession(Session& session)
{
    auto cwd = fs::current_path();
    auto crypticizierDirectory = cwd/fs::path(CRYPTICIZER);

    // Check for .crypticizer directory in the CWD
    if(fs::exists(crypticizierDirectory))
    {
        session.setSessionPath(cwd);
    }
}
