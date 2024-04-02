#ifndef __PKG_INSTALLER_H__
#define __PKG_INSTALLER_H__

#include "pkg_header.h"

namespace INSTALLER
{
    bool canInstallRemotePkg(const std::string &url);
    std::string getRemoteUrl(const std::string path, bool encodeUrl = false);
    int InstallRemotePkg(const std::string &path, PKGHeader *header);
    int InstallLocalPkg(const std::string &path, PKGHeader *header);
}

#endif