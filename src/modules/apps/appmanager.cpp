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

#include "appmanager.h"
#include "dfwatcher.h"
#include "alrecorder.h"
#include "basedir.h"

#include <QDebug>
#include <QDir>

AppManager::AppManager(QObject *parent)
 : QObject(parent)
 , watcher(new DFWatcher(this))
 , recorder(new AlRecorder(watcher, this))
{
    QStringList dataDirs;
    dataDirs << BaseDir::userAppDir().c_str();
    for (auto &dir : BaseDir::sysAppDirs())
        dataDirs << dir.c_str();

    recorder->WatchDirs(dataDirs);     // 监控应用desktop
}

AppManager::~AppManager()
{

}
