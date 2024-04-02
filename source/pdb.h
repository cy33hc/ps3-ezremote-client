#ifndef PKG_PDB_H
#define PKG_PDB_H

#include <string>
#include "pkg_header.h"

namespace PDB
{
    uint32_t GetNextInstallTaskId();
    uint32_t GetNextQueueTaskId();
    int CreateQueuePDBFiles(const std::string &pkg_task_folder, const std::string &url, PKGHeader *header);
    int CreateInstallPDBFiles(const std::string &pkg_task_folder, PKGHeader *header);
}

#endif