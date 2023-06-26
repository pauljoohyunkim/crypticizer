#include "../src/menu.h"
#include <string>
#include <vector>

int main()
{
    std::vector<std::string> entries = { "entry 1", "entry 2", "entry 3" };

    // Window Manager
    WindowManager wm;

    // First window
    auto winIndex = wm.createWindow(0, 0, 10, 10);
    //wrefresh(wm[winIndex]);


    // Menu Item
    //
    wm.makeMenu(winIndex, entries);
    getch();

    endwin();

    return 0;
}
