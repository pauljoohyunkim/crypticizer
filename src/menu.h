#ifndef MENU_H
#define MENU_H

#include <vector>
#include <ncurses.h>
#include <string>

#include "session.h"

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

        // Write menu onto a blank window (inc=true means the next time called,
        // entryIndex is incremented, otherwise decremented.
        //void makeMenu(unsigned int windowindex, std::vector<std::string> entries, bool inc=true);

        // Getter for window
        WINDOW* operator[] (unsigned int index);

        // Get Terminal Size
        void getTerminalSize(unsigned int& y, unsigned int& x);

        // Get Highlighted Window
        unsigned int getHighlightIndex();

        // Set Highlight
        void setHighlightIndex(unsigned int highlight);

    private:
        std::vector<WINDOW*> windows {};
        unsigned int highlighted { 0 };
};

class Menu
{
    public:
        Menu(WINDOW* awin, bool aboxed=true);

        void updateEntry(std::vector<std::string> aentries);
        void draw();
        void highlightNextEntry();
        void highlightPreviousEntry();
        void highlightFirstEntryInTheFrame();
        void highlightLastEntryInTheFrame();
        unsigned int getEntryIndex();

    private:
        WINDOW* win;
        // Dimension of the terminal
        unsigned int x;
        unsigned int y;
        int entryIndex { 0 };
        unsigned int entryIndexMin { 0 };
        unsigned int entryIndexMax { 0 };
        bool boxed { true };
        std::vector<std::string> entries {};

};

void menuUpdateFromSession(Session& session, Menu& menu);

#endif
