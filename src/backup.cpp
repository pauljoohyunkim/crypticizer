#include <git2.h>
#include <string>
#include <memory>
#include "backup.h"

Backupper::Backupper(std::string sessionPath)
{
    git_libgit2_init();
    repo = smart_git_repository_open(sessionPath);
    smart_git_revparse_ext(repo, parent, ref);
    index = smart_git_repository_index(repo);
    signature = smart_git_signature_default(repo);
}

Backupper::~Backupper()
{
    git_libgit2_shutdown();
}

void Backupper::stageFile(std::string filePath)
{
    smart_git_index_add_by_path(index, filePath);
}

void Backupper::stageFiles(std::vector<std::string> filePath_v)
{
    for (auto filePath : filePath_v)
    {
        stageFile(filePath);
    }
}

void Backupper::create_commit(std::string commitMessage)
{
    git_oid commit_oid;
    smart_git_index_write_tree(tree_oid, index);
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
        parent.get());
}
