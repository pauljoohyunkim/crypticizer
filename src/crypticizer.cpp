#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
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
#include "filestrhelper.h"

namespace fs = std::filesystem;

static void detectSession(Session& session, fs::path rootdir);
static std::string getPassword(bool verify=false);
static void newProjectMessage();
static void loadSession(Session& session);
static void launchSession(Session& session);
static void launchEditor(std::string textEditorProgram, std::string filename);
static void updatePreview(WINDOW* previewinterior, Menu& menu, Session& session);

Session crypticizerSession {};

int main(int argc, char** argv)
{
    std::cout << "crypticizer " << VERSION << std::endl;
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
    auto crypticizerDirectory { rootdir/fs::path(CRYPTICIZER) };
    auto hashfilepath { crypticizerDirectory/fs::path(HASHFILE) };
    auto editorfilepath { crypticizerDirectory/fs::path(EDITORFILE) };

    // Check for .crypticizer directory in the CWD
    if (fs::exists(crypticizerDirectory))
    {
        session.setSessionPath(rootdir, crypticizerDirectory, hashfilepath, editorfilepath);
    }
    else
    {
        // If not, create the .crypticizer directory in CWD
        // If this fails, halt the program.
        if (!fs::create_directory(crypticizerDirectory))
        {
            std::cerr << "Error: Could not create "
                      << crypticizerDirectory
                      << " directory. Make sure you have permission."
                      << std::endl;
            exit(CANNOT_CREATE_CRYPTICIZER_DIRECTORY);
        }
        newProjectMessage();
        session.setSessionPath(rootdir, crypticizerDirectory, hashfilepath, editorfilepath);
        std::cout << "Setting the password..." << std::endl;
        auto password = getPassword(true);
        session.setSessionPassword(password);
        // Create hash file
        Hasher hasher { HASHFUNCTION };
        hasher.generateSalt(HASH_SALT_N_BYTES);
        hasher.digestWithSalt(password);
        hasher.dumpHexdigestToFile(hashfilepath);
    }
    // Check if the hash file exists.
    if (fs::exists(hashfilepath))
    {
        // If it does, read it, and ask for password.
        auto referenceHasher = readHexdigestFile(hashfilepath, HASHFUNCTION, HASH_SALT_N_BYTES);
        std::cout << "Logging in..." << std::endl;
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
        session.setSessionPath(rootdir, crypticizerDirectory, hashfilepath, editorfilepath);
        auto password = getPassword(true);
        session.setSessionPassword(password);
        // Create hash file
        Hasher hasher { HASHFUNCTION };
        hasher.generateSalt(HASH_SALT_N_BYTES);
        hasher.digestWithSalt(password);
        hasher.dumpHexdigestToFile(hashfilepath);
    }

    // Check if the editor file exists.
    if (fs::exists(editorfilepath))
    {
        std::string editorfilecontentString { readFileToString(editorfilepath) };

        // Regex match to get one word.
        std::smatch match;
        std::regex stripFilter { "\\S+" };
        
        if (std::regex_search(editorfilecontentString, match, stripFilter))
        {
            session.setSessionTextEditor(match.str(0));
        }
        else
        {
            std::cout << "Warning: Could not recognize editor from " << editorfilepath
                << " so defaulting to using " << session.getSessionTextEditor() << "." << std::endl;
            std::cout << "Press Enter to continue." << std::endl;
            std::cin.ignore();
        }
    }
    else
    {
        std::string editorString {};
        // Ask if default text editor should be used.
        std::cout << "Currently the default text editor is " << session.getSessionTextEditor() << ". ";
        std::cout << "If you want to use another editor, enter it here, otherwise leave it blank: ";
        std::getline(std::cin, editorString);
        if (editorString.size())
        {
            // Attempt to get the first word
            editorString = editorString.substr(0, editorString.find(" "));
        }
        // Check again, as editorString originally could've just been whitespace.
        if (editorString.size())
        {
            session.setSessionTextEditor(editorString);
        }

        // Write default texteditor setting
        std::ofstream editorfileStream { editorfilepath };
        editorfileStream << session.getSessionTextEditor();
        editorfileStream.close();

        std::cout << "Session editor is set to " << session.getSessionTextEditor()
            << ". If you wish to change this, edit the " << editorfilepath << "." << std::endl
            << "Press Enter key to continue.";

        // Press enter key to continue
        std::cin.ignore();
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
    std::cout << std::endl;
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
    auto menuIndex { wm.createWindow(y - 3, x / 2, 0, 0) };
    Menu menu { wm[menuIndex] };

    // Preview Window (Box and Interior)
    auto previewBorderIndex { wm.createWindow(y - 3, x / 2, 0, x / 2) };
    auto previewBorder { wm[previewBorderIndex] };
    auto previewWinIndex { wm.createWindow(y - 5, x / 2 - 2, 1, x / 2 + 1, false) };
    auto previewWindow { wm[previewWinIndex] };
    wrefresh(previewBorder);
    wrefresh(previewWindow);

    // Info Window
    auto infoIndex { wm.createWindow(3, x, y-3, 0) };
    auto infoWin { wm[infoIndex] };
    mvwprintw(infoWin, 1, 1, "Info:\tArrow keys (or h,j,k,l):Navigate\t Enter:Edit\t+:New file\tc:Change Password\td:Dump Logs");
    wrefresh(infoWin);


    // Get menu from session
    menuUpdateFromSession(session, menu);
    menu.draw();
    updatePreview(previewWindow, menu, session);

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
            std::string textEditor { session.getSessionTextEditor() };
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
            // If there is no log, then just refresh,
            // otherwise, edit the highlighted
            if (session.getLogs().size() > 0)
            {
                std::string textEditor { session.getSessionTextEditor() };

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

                menu.highlightFirstEntryInTheFrame();
            }
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
        else if (c == 'c')
        {
            auto rootdir { session.getSessionPath() };
            auto crypticizerDirectory { session.getCrypticizerDirectory() };
            auto hashfilepath { session.getHashfilepath() };
            // Change password
            // Exit out of ncurses temporarily
            def_prog_mode();
            endwin();

            // Ask for current password.
            std::cout << "Resetting the password..." << std::endl;
            auto currentPass = getPassword();
            Hasher hasher { HASHFUNCTION };
            // Reading from hash file
            auto referenceHasher = readHexdigestFile(hashfilepath, HASHFUNCTION, HASH_SALT_N_BYTES);
            auto salt = referenceHasher.getSalt();
            // Set salt and digest
            hasher.setSalt(salt);
            hasher.digestWithSalt(currentPass);

            if (referenceHasher.hexsaltdigest() != hasher.hexsaltdigest())
            {
                //Password not matched!
                std::cerr << "Error: Wrong password!" << std::endl;
                std::cin.ignore();
            }
            else
            {
                // Ask for new password:
                std::cout << "Setting new password!" << std::endl;
                auto newPass = getPassword(true);
                auto logs = session.getLogs();
                LogCryptor lc_old { currentPass };
                LogCryptor lc_new { newPass };
                // For each of the logs, re-encrypt.
                for (auto log : logs)
                {
                    lc_old.setLog(log);
                    lc_new.setLog(log);

                    // Decrypt
                    auto tempPathString = lc_old.decrypt();

                    // Encrypt
                    lc_new.encrypt(tempPathString);
                }
                session.setSessionPassword(newPass);
                Hasher hasher { HASHFUNCTION };
                hasher.generateSalt(HASH_SALT_N_BYTES);
                hasher.digestWithSalt(newPass);
                hasher.dumpHexdigestToFile(hashfilepath);
            }


            // Restore
            reset_prog_mode();
            refresh();
        }
        else if (c == 'd')
        {
            // Exit out of ncurses temporarily
            def_prog_mode();
            endwin();

            std::cout << "Dumping password requires password verification" << std::endl;
            auto passwordInput = getPassword(false);
            if (passwordInput == session.getSessionPassword())
            {
                std::string dumpfilepathname;
                std::cout << "Enter the dump file name: " << std::endl;
                std::getline(std::cin, dumpfilepathname);
                session.dumpAsPlaintextFile(dumpfilepathname);
            }
            else
            {
                std::cerr << "Password not matched!" << std::endl;
                std::cin.ignore();
            }

            // Restore
            reset_prog_mode();
            refresh();
        }
        updatePreview(previewWindow, menu, session);
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

static void updatePreview(WINDOW* previewinterior, Menu& menu, Session& session)
{
    // Only do something if there is an entry highlighted.
    if (session.getLogs().size() > 0)
    {
        // Get highlited log
        LogCryptor lc { session.getSessionPassword() };
        lc.setLog(session.getLogs()[menu.getEntryIndex()]);
        writeTextInWindow(previewinterior, lc.decrypt(true));
    }
    else
    {
        writeTextInWindow(previewinterior, std::string("PREVIEW WILL BE SHOWN HERE!"));
    }
}
