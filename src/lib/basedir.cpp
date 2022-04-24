/*
 * Copyright (C) 2022 ~ 2023 Deepin Technology Co., Ltd.
 *
 * Author:     weizhixiang <weizhixiang@uniontech.com>
 *
 * Maintainer: weizhixiang <weizhixiang@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "basedir.h"
#include "dfile.h"
#include "dstring.h"

#include <algorithm>

BaseDir::BaseDir()
{

}

std::string BaseDir::homeDir()
{
    char *home = getenv("HOME");
    if (!home)
        return "";

    return std::string(home) + "/";
}

std::string BaseDir::uerDataDir()
{
    // default $HOME/.local/share
    std::string home = homeDir();
    std::string defaultDir = home.size() > 0 ? home + ".local/share/" : "";
    const char *xdgDataHomePtr = getenv("XDG_DATA_HOME");
    if (!xdgDataHomePtr)
        return defaultDir;

    if (!DFile::isAbs(xdgDataHomePtr))
        return defaultDir;

    return std::string(xdgDataHomePtr) + "/";
}

std::vector<std::string> BaseDir::sysDataDirs()
{
    std::vector<std::string> defaultDirs {"/usr/local/share/", "/usr/share/"};
    const char *xdgDataDirsPtr = getenv("XDG_DATA_DIRS");
    if (!xdgDataDirsPtr)
        return defaultDirs;

    std::string xdgDataDirsStr(xdgDataDirsPtr);
    std::vector<std::string> xdgDataDirs = DString::splitStr(xdgDataDirsStr, ':');
    if (xdgDataDirs.size() == 0)
        return defaultDirs;

    filterNotAbs(xdgDataDirs);
    addSuffixSlash(xdgDataDirs);
    return  xdgDataDirs;
}

std::string BaseDir::userConfigDir()
{
    // default $HOME/.config
    std::string defaultDir = homeDir() + ".config/";
    const char *xdgConfigHomePtr = getenv("XDG_CONFIG_HOME");
    if (!xdgConfigHomePtr)
        return defaultDir;

    std::string xdgConfigHome(xdgConfigHomePtr);
    if (!DFile::isAbs(xdgConfigHome))
        return defaultDir;

    return xdgConfigHome + "/";
}

std::vector<std::string> BaseDir::sysConfigDirs()
{
    std::vector<std::string> defaultDirs {"/etc/xdg/"};
    const char *xdgConfigDirsPtr = getenv("XDG_CONFIG_DIRS");
    if (!xdgConfigDirsPtr)
        return defaultDirs;

    std::string xdgConfigDirsStr(xdgConfigDirsPtr);
    std::vector<std::string> xdgConfigDirs = DString::splitStr(xdgConfigDirsStr, ':');
    if (xdgConfigDirs.size() == 0)
        return defaultDirs;

    filterNotAbs(xdgConfigDirs);
    addSuffixSlash(xdgConfigDirs);
    return xdgConfigDirs;
}

std::string BaseDir::userCacheDir()
{
    std::string home = homeDir();
    std::string defaultDir = home.size() > 0 ? home + ".cache/" : "";
    const char *xdgCacheHomePtr = getenv("XDG_CACHE_HOME");
    if (!xdgCacheHomePtr)
        return  defaultDir;

    std::string xdgCacheHome(xdgCacheHomePtr);
    if (!DFile::isAbs(xdgCacheHome))
        return defaultDir;

    return xdgCacheHome + "/";
}

std::string BaseDir::userAppDir()
{
    std::string dataDir = uerDataDir();
    return dataDir.size() > 0 ?  dataDir + "appliations/" : "";
}

std::vector<std::string> BaseDir::sysAppDirs()
{
    auto dataDirs = sysDataDirs();
    std::vector<std::string> sysAppDirs(dataDirs.size());
    std::transform(dataDirs.begin(), dataDirs.end(), sysAppDirs.begin(),
                   [](std::string dir) -> std::string {return dir + "applications/";});
    return sysAppDirs;
}

std::vector<std::string> BaseDir::appDirs()
{
    std::vector<std::string> appDirs = sysAppDirs();
    appDirs.push_back(userAppDir());
    return appDirs;
}

std::vector<std::string> BaseDir::autoStartDirs()
{
    std::vector<std::string> autoStartDirs = sysConfigDirs();
    autoStartDirs.push_back(userConfigDir());
    std::transform(autoStartDirs.begin(), autoStartDirs.end(), autoStartDirs.begin(),
                   [](std::string dir) -> std::string {return dir + "autostart/";});

    return autoStartDirs;
}

void BaseDir::filterNotAbs(std::vector<std::string> &dirs)
{
    for (auto iter = dirs.begin(); iter != dirs.end();) {       // erase element in vector
        if (!DFile::isAbs(*iter))
            iter = dirs.erase(iter);
        else
            iter++;
    }
}

void BaseDir::addSuffixSlash(std::vector<std::string> &dirs)
{
    for (auto &dir : dirs) {
        if (!DString::endWith(dir, "/"))
            dir += "/";
    }
}
