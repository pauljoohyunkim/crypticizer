#include <git2.h>
#include "../src/gitwrap.h"
#include "../src/backup.h"

int main()
{
    git_libgit2_init();

    {
        auto repo = smart_git_repository_open("temprepo");
        auto idx = smart_git_repository_index(repo);

        smart_git_index_add_by_path(idx, "testfile");
    }

    git_libgit2_shutdown();
    return 0;
}