#ifndef BACKUP_H
#define BACKUP_H

#include <git2.h>
#include <string>
#include <vector>
#include "gitwrap.h"

class Backupper
{
    public:
        Backupper(std::string sessionPath);
        ~Backupper();
        void stageFile(std::string filePath);
        void stageFiles(std::vector<std::string> filePath_v);

        SMART_GIT_WRAP(git_repository) repo { nullptr, nullptr };
        SMART_GIT_WRAP(git_index) index { nullptr, nullptr };
        SMART_GIT_WRAP(git_object) parent { nullptr, nullptr };
        SMART_GIT_WRAP(git_reference) ref { nullptr, nullptr };
        SMART_GIT_WRAP(git_signature) signature { nullptr, nullptr };
        git_oid tree_oid;

    private:
};

#endif