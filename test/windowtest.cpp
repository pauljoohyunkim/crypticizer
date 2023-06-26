#include "../src/menu.h"
#include <string>
#include <vector>

int main()
{
    std::vector<std::string> entries = { "entry 1", "entry 2", "entry 3" };
    keypad(stdscr, TRUE);

    // Window Manager
    WindowManager wm;

    // First window
    auto winIndex = wm.createWindow(10, 6, 10, 10);
    //wrefresh(wm[winIndex]);


    // Menu Item
    auto entryIndex = 0;
    while (auto c = getch())
    {
        bool inc;
        if (c == 'j')
        {
            inc = true;
        }
        else if (c == 'k')
        {
            inc = false;
        }
        else if (c == 'q')
        {
            break;
        }
        wm.makeMenu(winIndex, entries, inc);
    }
    

    endwin();

    return 0;
}
