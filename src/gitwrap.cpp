#include <string>
#include <memory>
#include <git2.h>
#include "gitwrap.h"

/* Open git repository */
std::unique_ptr<git_repository, void(*)(git_repository*)> smart_git_repository_open(std::string repoPath)
{
    git_repository* repo;
    auto error { git_repository_open(&repo, repoPath.c_str()) };

    auto smart_repo { std::unique_ptr<git_repository, void(*)(git_repository*)>(repo, smart_git_repository_free) };
    return smart_repo;
}

/* 
 * Destructors
 * Not to be explicitly called.
*/

void smart_git_repository_free(git_repository* repo)
{
    git_repository_free(repo);
}