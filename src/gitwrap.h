#ifndef GITWRAP_H
#define GITWRAP_H

/* It is recommended to follow the following format when using functions here:
git_libgit2_init();
{
    SOME CODE INVOLVING THE FUNCTIONS BELOW
}
git_libgit2_shutdown();

Or use it in a class with constructor using git_libgit2_init()
and destructor using git_libgit2_shutdown()

so that the scope can take care of destructors in order.
*/

#include <memory>
#include <string>
#include <git2.h>

#define SMART_GIT_WRAP(x) std::unique_ptr<x, void(*)(x*)>

SMART_GIT_WRAP(git_repository) smart_git_repository_open(std::string repoPath);
SMART_GIT_WRAP(git_index) smart_git_repository_index(SMART_GIT_WRAP(git_repository)& repo);
void smart_git_index_add_by_path(SMART_GIT_WRAP(git_index)& index, std::string filepath);
SMART_GIT_WRAP(git_signature) smart_git_signature_default(SMART_GIT_WRAP(git_repository)& repo);
void smart_git_index_write_tree(git_oid& tree_oid, SMART_GIT_WRAP(git_index)& index);
SMART_GIT_WRAP(git_tree) smart_git_tree_lookup(SMART_GIT_WRAP(git_repository)& repo, git_oid& tree_oid);


//void smart_git_repository_free(git_repository* repo);
//void smart_git_index_free(git_index* idx);

#endif