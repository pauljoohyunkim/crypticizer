#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <termios.h>
#include <sys/wait.h>
#include <regex>
#include "crypticizer.h"
#include "cryptor.h"
#include "session.h"
#include "errorcodes.h"
#include "menu.h"

namespace fs = std::filesystem;

static void detectSession(Session& session, fs::path rootdir);
static std::string getPassword(bool verify=false);
static void newProjectMessage();
static void loadSession(Session& session);
static void launchSession(Session& session);
static void launchEditor(std::string textEditorProgram, std::string filename);

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

    clear();
    endwin();

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
    auto hashfilepath { crypticizierDirectory/fs::path(HASHFILE) };

    // Check for .crypticizer directory in the CWD
    if (fs::exists(crypticizierDirectory))
    {
        session.setSessionPath(rootdir);
        // Check if the hash file exists.
        if (fs::exists(hashfilepath))
        {
            // If it does, read it, and ask for password.
            auto referenceHasher = readHexdigestFile(hashfilepath, HASHFUNCTION, HASH_SALT_N_BYTES);
            auto password = getPassword(false);
            Hasher hasher { HASHFUNCTION };
            auto salt = referenceHasher.getSalt();
            // Set salt and digest
            hasher.setSalt(salt);
            hasher.digestWithSalt(password);
            if (referenceHasher.hexsaltdigest() != hasher.hexsaltdigest())
            {
                //Password not matched!
                std::cerr << "Error: Wrong password!" << std::endl;
                exit(WRONG_PASSWORD);
            }
            else
            {
                session.setSessionPassword(password);
            }
        }
        else
        {
            //Otherwise, create hashfile

            newProjectMessage();
            session.setSessionPath(rootdir);
            auto password = getPassword(true);
            session.setSessionPassword(password);
            // Create hash file
            Hasher hasher { HASHFUNCTION };
            hasher.generateSalt(HASH_SALT_N_BYTES);
            hasher.digestWithSalt(password);
            hasher.dumpHexdigestToFile(hashfilepath);
        }

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
        newProjectMessage();
        session.setSessionPath(rootdir);
        auto password = getPassword(true);
        session.setSessionPassword(password);
        // Create hash file
        Hasher hasher { HASHFUNCTION };
        hasher.generateSalt(HASH_SALT_N_BYTES);
        hasher.digestWithSalt(password);
        hasher.dumpHexdigestToFile(hashfilepath);
    }
}

static std::string getPassword(bool verify)
{
    std::cout << "Enter password: " << std::flush;

    termios old_settings, new_settings;
    tcgetattr(STDIN_FILENO, &old_settings);
    new_settings = old_settings;
    new_settings.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

    std::string pass;
    std::getline(std::cin, pass);

    if (verify)
    {
        std::string pass2;
        std::cout << std::endl << "Enter the password again: " << std::flush;
        std::getline(std::cin, pass2);
        if (pass != pass2)
        {
            std::cout << std::endl;
            std::cerr << "Error: Passwords do not match!" << std::endl;
            tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
            exit(PASSWORD_DO_NOT_MATCH);
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
    return pass;
}

static void newProjectMessage()
{
    std::cout << "Creating new project..." << std::endl;
    std::cout << "WARNING: It is YOUR responsibility to remember your own damn password!" << std::endl;
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
            session.addLog(timestamp);
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
    auto menuIndex { wm.createWindow(y - 3, x, 0, 0) };
    Menu menu { wm[menuIndex] };
    // Info Window
    auto infoIndex { wm.createWindow(3, x, y-3, 0) };
    auto infoWin { wm[infoIndex] };
    mvwprintw(infoWin, 1, 1, "Info:\tArrow keys (or h,j,k,l):Navigate\t Enter:Edit\t+:New file");
    wrefresh(infoWin);


    // Get menu from session
    menuUpdateFromSession(session, menu);
    menu.draw();

    // Wiring of the keys!
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
        else if (c == 'l' || c == KEY_RIGHT)
        {
            menu.highlightLastEntryInTheFrame();
            menu.highlightNextEntry();
        }
        else if (c == 'h' || c == KEY_LEFT)
        {
            menu.highlightFirstEntryInTheFrame();
            menu.highlightPreviousEntry();
            menu.highlightFirstEntryInTheFrame();
        }
        else if (c == KEY_HOME)
        {
            menu.highlightFirstEntryInTheFrame();
        }
        else if (c == KEY_END)
        {
            menu.highlightLastEntryInTheFrame();
        }
        else if (c == '+')
        {
            // Creating new log
            std::string textEditor { "vim" };
            LogCryptor lc { session.getSessionPassword() };
            Log log { session.getSessionPath() };
            lc.setLog(log);

            // Create new temporary file
            std::string tempentryPathString = lc.createTempFile();
            // Allow editing
            launchEditor(textEditor, tempentryPathString);

            // Encrypt
            lc.encrypt();

            // Refresh
            loadSession(session);
            menuUpdateFromSession(session, menu);
        }
        else if (c == '\n')
        {
            std::string textEditor { "vim" };

            // Get existing log
            LogCryptor lc { session.getSessionPassword() };
            lc.setLog(session.getLogs()[menu.getEntryIndex()]);
            //std::string filename { session.getLogs()[menu.getEntryIndex()].logpath.string() };
            
            // Decrypting
            auto tempentryPathString = lc.decrypt();

            // Edit
            launchEditor(textEditor, tempentryPathString);

            // Encrypt
            lc.encrypt();

            // Refresh
            loadSession(session);
            menuUpdateFromSession(session, menu);
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

static void launchEditor(std::string textEditorProgram, std::string filename)
{

    def_prog_mode();
    endwin();
    // Fork and exec to create child process to the text editor.
    auto pid { fork() };

    if (pid == 0)
    {
        // Child
        execlp(textEditorProgram.c_str(), textEditorProgram.c_str(), filename.c_str(), NULL);
    }
    else if (pid > 0)
    {
        // Parent
        wait(0);
        reset_prog_mode();
        refresh();
    }
}
