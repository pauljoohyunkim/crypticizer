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
    auto winIndex = wm.createWindow(0, 0, 10, 10);
    //wrefresh(wm[winIndex]);


    // Menu Item
    auto entryIndex = 0;
    while (auto c = getch())
    {
        if (c == 'j')
        {
            entryIndex++;
        }
        else if (c == 'k')
        {
            entryIndex--;
        }
        else if (c == 'q')
        {
            break;
        }
        wm.makeMenu(winIndex, entries, entryIndex);
    }
    

    endwin();

    return 0;
}
