#ifndef MENU_H
#define MENU_H

#include <vector>
#include <ncurses.h>
#include <memory>

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
                unsigned start_x);

        // Getter for window
        WINDOW* operator[] (unsigned int index);
    private:

        std::vector<WINDOW*> windows {};
};

#endif
