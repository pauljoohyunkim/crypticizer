#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include "crypticizer.h"
#include "session.h"
#include "errorcodes.h"

namespace fs = std::filesystem;

static void detectSession(Session& session, fs::path rootdir);

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
        fs::path rootdir { argv[1] };
        detectSession(crypticizerSession, rootdir);
    }
    else
    {
        detectSession(crypticizerSession, fs::current_path());
    }

    return EXIT_SUCCESS;
}

// If there exists .crypticizer directory,
static void detectSession(Session& session, fs::path rootdir)
{
    // Check if root directory exists, if not, create it
    if (!fs::exists(rootdir) && !fs::create_directory(rootdir))
    {
            std::cerr << "Error: Could not create "
                      << rootdir
                      << " directory. Make sure you have permission."
                      << std::endl;
            exit(CANNOT_CREATE_CRYPTICIZER_DIRECTORY);
    }
    auto crypticizierDirectory { rootdir/fs::path(CRYPTICIZER) };

    // Check for .crypticizer directory in the CWD
    if (fs::exists(crypticizierDirectory))
    {
        session.setSessionPath(rootdir);
    }
    else
    {
        // If not, create the .crypticizer directory in CWD
        // If this fails, halt the program.
        if (!fs::create_directory(crypticizierDirectory))
        {
            std::cerr << "Error: Could not create "
                      << crypticizierDirectory
                      << " directory. Make sure you have permission."
                      << std::endl;
            exit(CANNOT_CREATE_CRYPTICIZER_DIRECTORY);
        }
    }
}
