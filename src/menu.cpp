#include "menu.h"
#include <vector>
#include <ncurses.h>
#include <memory>

WindowManager::WindowManager()
{
    initscr();
    raw();
    noecho();
}

WindowManager::~WindowManager()
{
    endwin();
}

unsigned int WindowManager::createWindow(unsigned int height,
        unsigned int width,
        unsigned int start_y,
        unsigned start_x)
{
    WINDOW* win = newwin(height, width, start_y, start_x);
    unsigned int index = windows.size();

    windows.push_back(win);
    return index;
}
