#include "../src/menu.h"
#include <string>
#include <vector>

int main()
{
    std::vector<std::string> entries = { "1 entry",
                                         "2 entry",
                                         "3 entry",
                                         "4 entry",
                                         "5 entry",
                                         "6 entry"
                                       };
    keypad(stdscr, TRUE);

    // Window Manager
    WindowManager wm;

    // First window
    auto winIndex = wm.createWindow(6, 6, 10, 0);

    // Menu
    Menu menu { wm[0] };
    menu.updateEntry(entries);
    //wrefresh(wm[winIndex]);


    // Menu Item
    while (auto c = getch())
    {
        bool inc;
        if (c == 'j')
        {
            menu.highlightNextEntry();
            menu.draw();
        }
        else if (c == 'k')
        {
            menu.highlightPreviousEntry();
            menu.draw();
        }
        else if (c == '\n')
        {
            printw("%d\n", menu.getEntryIndex());
        }
        else if (c == 'q')
        {
            break;
        }
    }
    

    endwin();

    return 0;
}
