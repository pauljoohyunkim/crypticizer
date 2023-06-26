#include "menu.h"
#include <vector>
#include <iostream>
#include <ncurses.h>
#include <string>

WindowManager::WindowManager()
{
    initscr();
    raw();
    noecho();
    refresh();
}

WindowManager::~WindowManager()
{
    std::cout << "Calling destructor" << std::endl;
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
    wrefresh(win);
    return index;
}

WINDOW* WindowManager::operator[](unsigned int index)
{
    return windows[index];
}

void WindowManager::getTerminalSize(unsigned int& y, unsigned int& x)
{
    getmaxyx(stdscr, y, x);
}

void WindowManager::makeMenu(unsigned int windowindex, std::vector<std::string> entries, int entryIndex)
{
    auto entrySize { entries.size() };
    entryIndex = (entryIndex % entrySize + entrySize) % entrySize;
    auto win { windows[windowindex] };

    // Write entries
    for (unsigned int rownum = 1; rownum <= entries.size(); rownum++)
    {
        if ((int) rownum - 1 == entryIndex)
        {
            wattron(win, A_STANDOUT);
        }
        else
        {
            wattroff(win, A_STANDOUT);
        }
        mvwprintw(win, rownum, 1, entries[rownum-1].c_str());
    }

    wrefresh(win);
}
