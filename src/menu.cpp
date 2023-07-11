#include "menu.h"
#include <ctime>
#include <vector>
#include <iostream>
#include <ncurses.h>
#include <algorithm>
#include <string>

WindowManager::WindowManager()
{
    setlocale(LC_ALL, "");
    initscr();
    raw();
    noecho();
    refresh();
    keypad(stdscr, TRUE);
    curs_set(0);
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

unsigned int WindowManager::getHighlightIndex()
{
    return highlighted;
}

void WindowManager::setHighlightIndex(unsigned int highlight)
{
    if (highlight < windows.size())
    {
        highlighted = highlight;
    }
    else
    {
        highlighted = 0;
    }
}

/*
void WindowManager::makeMenu(unsigned int windowindex, std::vector<std::string> entries, bool inc)
{
    auto entrySize { entries.size() };
    static int entryIndex { -1 };
    if (inc)
    {
        entryIndex++;
    }
    else
    {
        entryIndex--;
    }
    entryIndex = (entryIndex % entrySize + entrySize) % entrySize;
    auto win { windows[windowindex] };

    // DEBUG
    printw("%d", entryIndex);

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
*/

Menu::Menu(WINDOW* awin, bool aboxed)
{
    win = awin;
    getmaxyx(win, y, x);
    boxed = aboxed;
}

void Menu::updateEntry(std::vector<std::string> aentries)
{
    entries = aentries;
    entryIndexMin = 0;
    getmaxyx(win, y, x);
    if (entries.size() == 0)
    {
        entryIndexMax = 0;
    }
    else
    {
        entryIndexMax = std::min<unsigned int>(entries.size() - 1, y - 3);
    }
}

void Menu::draw()
{
    wclear(win);
    auto entrySize { entries.size() };
    int entrySizeInt { (int) entrySize };
    if (entrySizeInt == 0)
    {
        // If no entry, then do nothing!
        return;
    }
    entryIndex = (entryIndex % entrySizeInt + entrySizeInt) % entrySizeInt;
    //printw("entryIndex: %d\t in [%d, %d]\t y:%d\n", entryIndex, entryIndexMin, entryIndexMax, y);
    // Get dimension of the window for the case that
    // there might be exceedingly many and long entries.
    getmaxyx(win, y, x);
    
    // Write entries
    for (unsigned int index = entryIndexMin; index <= entryIndexMax; index++)
    {
        auto rownum = index - entryIndexMin + 1;
        //if ((int) rownum - 1 == (int) entryIndex)
        if (index == (unsigned int) entryIndex)
        {
            wattron(win, A_STANDOUT);
        }
        else
        {
            wattroff(win, A_STANDOUT);
        }
        // Truncate the entry to show if it is too long for the box.
        auto entry { entries[index] };
        auto entryView { entry.substr(0, std::min<unsigned int>(entry.length(), x - 2)) };
        mvwprintw(win, rownum, 1, "%s", entryView.c_str());
        wattroff(win, A_STANDOUT);
    }

    if (boxed)
    {
        box(win, 0, 0);
    }
    wrefresh(win);
}

void Menu::highlightNextEntry()
{
    getmaxyx(win, y, x);
    auto num_of_entries_in_menu = y - 2;
    // Only if there is the next element.
    if (entryIndex < (int)entries.size()-1)
    {
        entryIndex++;
        entryIndexMin = (entryIndex / num_of_entries_in_menu) * num_of_entries_in_menu;
        entryIndexMax = std::min<unsigned int>(entryIndexMin + num_of_entries_in_menu - 1, entries.size()-1);
    }

}

void Menu::highlightPreviousEntry()
{
    getmaxyx(win, y, x);
    auto num_of_entries_in_menu = y - 2;
    // Only if there is the previous element.
    if (entryIndex > 0)
    {
        entryIndex--;
        entryIndexMin = (entryIndex / num_of_entries_in_menu) * num_of_entries_in_menu;
        entryIndexMax = std::min<unsigned int>(entries.size() - 1, entryIndexMin + num_of_entries_in_menu - 1);
    }
    
}


void Menu::highlightFirstEntryInTheFrame()
{
    entryIndex = entryIndexMin;
}

void Menu::highlightLastEntryInTheFrame()
{
    entryIndex = entryIndexMax;
}

unsigned int Menu::getEntryIndex()
{
    return entryIndex;
}

void menuUpdateFromSession(Session& session, Menu& menu)
{
    std::vector<std::string> menuEntries {};
    // Fill the menu
    for (auto log : session.getLogs())
    {
        menuEntries.push_back(log.getLocalTime());
        //menuEntries.push_back(log.logpath.string());
    }
    menu.updateEntry(menuEntries);
}

void writeTextInWindow(WINDOW* win, std::string text)
{
    // Clear window
    wclear(win);

    // Write
    wprintw(win, "%s", text.c_str());

    // Refresh
    wrefresh(win);
}
