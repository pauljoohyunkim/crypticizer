#include <git2.h>
#include "../src/gitwrap.h"
#include "../src/backup.h"

int main(int argc, char** argv)
{
    git_libgit2_init();

    {
        /*
        auto repo = smart_git_repository_open(argv[1]);
        SMART_GIT_WRAP(git_object) parent { nullptr, nullptr };
        SMART_GIT_WRAP(git_reference) ref { nullptr, nullptr };
        smart_git_revparse_ext(repo, parent, ref);

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
            parent.get() ? 1 : 0,
            parent.get()
        );
        */
        Backupper backupper{argv[1]};
        backupper.stageFile("testfile1");
        backupper.stageFile("testfile2");
        backupper.stageFile("testfile3");
    }

    git_libgit2_shutdown();
    return 0;
}