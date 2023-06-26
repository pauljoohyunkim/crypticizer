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
    private:
        // Creates window and returns the index.
        unsigned int createWindow(unsigned int height,
                unsigned int width,
                unsigned int start_y,
                unsigned start_x);

        std::vector<WINDOW*> windows {};
};

#endif
