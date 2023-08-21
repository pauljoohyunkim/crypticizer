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
        git_oid commit_oid, tree_oid;

        smart_git_index_add_by_path(idx, "testfile1");
        smart_git_index_add_by_path(idx, "testfile2");
        smart_git_index_add_by_path(idx, "testfile3");
        smart_git_index_add_by_path(idx, "repodir/testfile4");
        
        smart_git_index_write_tree(tree_oid, idx);
        auto tree = smart_git_tree_lookup(repo, tree_oid);
        git_commit_create_v(
            &commit_oid,
            repo.get(),
            "HEAD",
            signature.get(),
            signature.get(),
            NULL,
            "test commit",
            tree.get(),
            0
        );
    }

    git_libgit2_shutdown();
    return 0;
}