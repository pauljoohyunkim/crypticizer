#include <git2.h>
#include "../src/gitwrap.h"
#include "../src/backup.h"

int main()
{
    git_libgit2_init();

    {
        auto repo = smart_git_repository_open("..");
    }

    git_libgit2_shutdown();
    return 0;
}