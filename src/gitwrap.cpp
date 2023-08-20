#include <string>
#include <memory>
#include <git2.h>
#include "gitwrap.h"

static void smart_git_repository_free(git_repository* repo);
static void smart_git_index_free(git_index* idx);
static void smart_git_signature_free(git_signature* signature);
static void smart_git_tree_free(git_tree* tree);

/* Open git repository */
SMART_GIT_WRAP(git_repository) smart_git_repository_open(std::string repoPath)
{
    git_repository* repo;
    auto error { git_repository_open(&repo, repoPath.c_str()) };

    auto smart_repo { SMART_GIT_WRAP(git_repository)(repo, smart_git_repository_free) };
    return smart_repo;
}
/* Get repo index */
SMART_GIT_WRAP(git_index) smart_git_repository_index(SMART_GIT_WRAP(git_repository)& repo)
{
    git_index* idx;
    auto error { git_repository_index(&idx, repo.get()) };

    auto smart_index { SMART_GIT_WRAP(git_index)(idx, smart_git_index_free) };
    return smart_index;
}
/* Add file */
void smart_git_index_add_by_path(SMART_GIT_WRAP(git_index)& index, std::string filepath)
{
    git_index_add_bypath(index.get(), filepath.c_str());
    git_index_write(index.get());
}

SMART_GIT_WRAP(git_signature) smart_git_signature_default(SMART_GIT_WRAP(git_repository)& repo)
{
    git_signature* signature;
    auto error { git_signature_default(&signature, repo.get()) };

    auto smart_signature { SMART_GIT_WRAP(git_signature)(signature, smart_git_signature_free) };
    return smart_signature;
}
SMART_GIT_WRAP(git_tree) smart_git_tree_lookup(SMART_GIT_WRAP(git_repository) repo, git_oid oid)
{
    git_tree* tree;
    auto error { git_tree_lookup(&tree, repo.get(), &oid) };

    auto smart_tree { SMART_GIT_WRAP(git_tree)(tree, smart_git_tree_free) };
    return smart_tree;
}


//void smart_commit(SMART_GIT_WRAP(git_repository) &repo, SMART_GIT_WRAP(git_index) &index)
//{
//
//}

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
static void smart_git_signature_free(git_signature* signature)
{
    git_signature_free(signature);
}
static void smart_git_tree_free(git_tree* tree)
{
    git_tree_free(tree);
}