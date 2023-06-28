#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <regex>
#include "crypticizer.h"
#include "session.h"
#include "errorcodes.h"
#include "menu.h"

namespace fs = std::filesystem;

static void detectSession(Session& session, fs::path rootdir);
static void loadSession(Session& session);
static void launchSession(Session& session);

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

    // Load from crpt files
    loadSession(crypticizerSession);

    // Launch the terminal-based UI
    launchSession(crypticizerSession);

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
        session.setSessionPath(rootdir);
    }
}

static void loadSession(Session& session)
{
    session.clearLog();
    auto rootdir { session.getSessionPath() };
    const std::regex filter { "[0-9]+\\.crpt$" };

    // Filter out files with extension .crpt
    for (auto filepath : fs::directory_iterator{rootdir})
    {
        std::smatch match;
        auto pathString { filepath.path().filename().string() };
        if (std::regex_search(pathString, match, filter))
        {
            auto fp { filepath.path() };
            auto timestamp { (std::time_t) std::stoi(fp.stem()) };
            session.addLog(fp, timestamp);
        }
    }
}

static void launchSession(Session& session)
{
    WindowManager wm;
    // Window size
    int x, y;
    getmaxyx(stdscr, y, x);
    // Entry Window
    auto menuIndex { wm.createWindow(y / 2, x, 0, 0) };
    Menu menu { wm[menuIndex] };
    // Info Window
    auto infoIndex { wm.createWindow(y / 2, x, y / 2, 0) };

    // Get menu from session
    menuUpdateFromSession(session, menu);
    menu.draw();
    auto c = getch();
    while (true)
    {
        if (c == 'q')
        {
            break;
        }
        else if (c == 'j' || c == KEY_DOWN)
        {
            menu.highlightNextEntry();
        }
        else if (c == 'k' || c == KEY_UP)
        {
            menu.highlightPreviousEntry();
        }
        else if (c == '\n')
        {
            printw("%d", menu.getEntryIndex());
        }
        else if (c == KEY_F(5))
        {
            // Refresh
            loadSession(session);
            menuUpdateFromSession(session, menu);
        }
        menu.draw();
        c = getch();
    }
}
