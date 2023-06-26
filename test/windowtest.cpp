#include "../src/menu.h"

int main()
{
    WindowManager wm;
    auto i = wm.createWindow(0, 0, 10, 10);
    getch();

    auto win =  wm[0];

    return 0;
}
