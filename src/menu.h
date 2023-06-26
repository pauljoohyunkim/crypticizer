#ifndef MENU_H
#define MENU_H

#include <vector>
#include <ncurses.h>
#include <string>

class WindowManager
{
    public:
        // Starts ncurses
        WindowManager();
        // Ends ncurses
        ~WindowManager();
        // Creates window and returns the index.
        unsigned int createWindow(unsigned int height,
                unsigned int width,
                unsigned int start_y,
                unsigned start_x, bool boxed=true);

        // Write menu onto a blank window
        void makeMenu(unsigned int windowindex, std::vector<std::string> entries);

        // Getter for window
        WINDOW* operator[] (unsigned int index);

        // Get Terminal Size
        void getTerminalSize(unsigned int& y, unsigned int& x);
    private:
        std::vector<WINDOW*> windows {};
};

#endif
