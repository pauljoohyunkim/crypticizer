#include "menu.h"
#include <vector>
#include <ncurses.h>
#include <memory>

WindowManager::WindowManager()
{
    initscr();
    raw();
    noecho();
    refresh();
}

WindowManager::~WindowManager()
{
    endwin();
}

unsigned int WindowManager::createWindow(unsigned int height,
        unsigned int width,
        unsigned int start_y,
        unsigned start_x, bool boxed)
{
    WINDOW* win = newwin(height, width, start_y, start_x);
    unsigned int index = windows.size();

    windows.push_back(win);

    if (boxed)
    {
        box(win, 0, 0);
    }
    return index;
}

WINDOW* WindowManager::operator[](unsigned int index)
{
    return windows[index];
}
