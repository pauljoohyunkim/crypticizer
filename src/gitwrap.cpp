#include <iostream>
#include <string>
#include <memory>
#include <git2.h>
#include "gitwrap.h"

// Reference for implementing commit:
// https://stackoverflow.com/questions/27672722/libgit2-commit-example
// https://libgit2.org/libgit2/ex/HEAD/commit.html
// https://github.com/libgit2/libgit2/blob/091165c53b2bcd5d41fb71d43ed5a23a3d96bf5d/tests/object/commit/commitstagedfile.c#L21-L134

static void smart_git_repository_free(git_repository* repo);
static void smart_git_index_free(git_index* idx);
static void smart_git_signature_free(git_signature* signature);
static void smart_git_tree_free(git_tree* tree);
static void smart_git_object_free(git_object* obj);
static void smart_git_reference_free(git_reference* ref);

/* Open git repository */
SMART_GIT_WRAP(git_repository) smart_git_repository_open(std::string repoPath)
{
    git_repository* repo { nullptr };
    auto error { git_repository_open(&repo, repoPath.c_str()) };
    if (error)
    {
        std::cerr << "smart_git_repository_open: " << error << std::endl;
    }

    auto smart_repo { SMART_GIT_WRAP(git_repository)(repo, smart_git_repository_free) };
    return smart_repo;
}
/* Get repo index */
SMART_GIT_WRAP(git_index) smart_git_repository_index(SMART_GIT_WRAP(git_repository)& repo)
{
    git_index* idx { nullptr };
    auto error { git_repository_index(&idx, repo.get()) };
    if (error)
    {
        std::cerr << "smart_git_repository_index: " << error << std::endl;
    }

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
    git_signature* signature { nullptr };
    auto error { git_signature_default(&signature, repo.get()) };
    if (error)
    {
        std::cerr << "smart_git_signature_default: " << error << std::endl;
    }

    auto smart_signature { SMART_GIT_WRAP(git_signature)(signature, smart_git_signature_free) };
    return smart_signature;
}

void smart_git_index_write_tree(git_oid& tree_oid, SMART_GIT_WRAP(git_index)& index)
{
    auto error { git_index_write_tree(&tree_oid, index.get()) };
    if (error)
    {
        std::cerr << "smart_git_index_write_tree: " << error << std::endl;
    }

}

SMART_GIT_WRAP(git_tree) smart_git_tree_lookup(SMART_GIT_WRAP(git_repository)& repo, git_oid& tree_oid)
{
    git_tree* tree { nullptr };
    auto error { git_tree_lookup(&tree, repo.get(), &tree_oid) };
    if (error)
    {
        std::cerr << "smart_git_tree_lookup: " << error << std::endl;
    }

    auto smart_tree { SMART_GIT_WRAP(git_tree) (tree, smart_git_tree_free) };
    return smart_tree;
}

void smart_git_revparse_ext(SMART_GIT_WRAP(git_repository)& repo, SMART_GIT_WRAP(git_object)& parent, SMART_GIT_WRAP(git_reference)& ref)
{
    git_object* raw_parent { nullptr };
    git_reference* raw_ref { nullptr };
    auto error { git_revparse_ext(&raw_parent, &raw_ref, repo.get(), "HEAD") };
    if (error == GIT_ENOTFOUND)
    {
        std::cerr << "HEAD not found. Creating first commit" << std::endl;
    }
    else if (error)
    {
        std::cerr << "smart_git_revparse_ext: " << error << std::endl;
    }

    parent = SMART_GIT_WRAP(git_object)(raw_parent, smart_git_object_free);
    ref = SMART_GIT_WRAP(git_reference)(raw_ref, smart_git_reference_free);
}


//void smart_commit(
//    std::unique_ptr<git_repository, void (*)(git_repository *)> &repo,
//    std::unique_ptr<git_index, void (*)(git_index *)> &index)
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
static void smart_git_object_free(git_object* obj)
{
    git_object_free(obj);
}
static void smart_git_reference_free(git_reference* ref)
{
    git_reference_free(ref);
}