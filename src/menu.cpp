#include "menu.h"
#include <vector>
#include <iostream>
#include <ncurses.h>
#include <algorithm>
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

    // Get dimension of the window for the case that
    // there might be exceedingly many and long entries.
    unsigned int x, y;
    getmaxyx(win, y, x);
    //unsigned int rownumLeft;

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
        // Truncate the entry to show if it is too long for the box.
        auto entry { entries[rownum-1] };
        auto entryView { entry.substr(0, std::min<unsigned int>(entry.length(), x) - 2) };
        mvwprintw(win, rownum, 1, entryView.c_str());
    }

    wrefresh(win);
}
