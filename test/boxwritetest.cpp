#include <string>
#include <vector>

#include "../src/menu.h"
#include "../src/session.h"

int main()
{
    WindowManager wm;
    auto index { wm.createWindow(20, 20, 0, 0) };
    auto win { wm[index] };

    writeTextInBox(win, std::string("Hello World, my name is Paul\nYolo schmuck."));

    getch();
}
