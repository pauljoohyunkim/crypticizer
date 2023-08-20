#include <git2.h>
#include "../src/gitwrap.h"
#include "../src/backup.h"

int main()
{
    git_libgit2_init();

    {
        auto repo = smart_git_repository_open("temprepo");
        auto idx = smart_git_repository_index(repo);
        auto signature = smart_git_signature_default(repo);

        smart_git_index_add_by_path(idx, "testfile1");
        smart_git_index_add_by_path(idx, "testfile2");
        smart_git_index_add_by_path(idx, "testfile3");
        smart_git_index_add_by_path(idx, "repodir/testfile4");

    }

    git_libgit2_shutdown();
    return 0;
}