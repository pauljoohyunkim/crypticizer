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

std::unique_ptr<git_repository, void(*)(git_repository*)> smart_git_repository_open(std::string repoPath);


void smart_git_repository_free(git_repository* repo);

#endif