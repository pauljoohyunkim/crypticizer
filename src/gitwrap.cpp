#include <string>
#include <memory>
#include <git2.h>
#include "gitwrap.h"

static void smart_git_repository_free(git_repository* repo);
static void smart_git_index_free(git_index* idx);

/* Open git repository */
std::unique_ptr<git_repository, void(*)(git_repository*)>
smart_git_repository_open(std::string repoPath)
{
    git_repository* repo;
    auto error { git_repository_open(&repo, repoPath.c_str()) };

    auto smart_repo { std::unique_ptr<git_repository, void(*)(git_repository*)>(repo, smart_git_repository_free) };
    return smart_repo;
}
/* Get repo index */
std::unique_ptr<git_index, void(*)(git_index*)>
smart_git_repository_index(std::unique_ptr<git_repository, void(*)(git_repository*)>& repo)
{
    git_index* idx;
    auto error { git_repository_index(&idx, repo.get()) };

    auto smart_repo { std::unique_ptr<git_index, void(*)(git_index*)>(idx, smart_git_index_free) };
    return smart_repo;
}
/* Add file */
void smart_git_index_add_by_path(std::unique_ptr<git_index, void(*)(git_index*)>& index, std::string filepath)
{
    git_index_add_bypath(index.get(), filepath.c_str());
    git_index_write(index.get());
}

/* 
 * Destructors
 * Not to be explicitly called.
*/

static void smart_git_repository_free(git_repository* repo)
{
    git_repository_free(repo);
}
static void smart_git_index_free(git_index* idx)
{
    git_index_free(idx);
}