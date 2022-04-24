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

#ifndef ALRECORDER_H
#define ALRECORDER_H

#include <QObject>
#include <QMap>
#include <QMutex>

class DFWatcher;

// 记录应用状态信息
class AlRecorder: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.daemon.AlRecorder1")

public:
    struct subRecorder {
        QString statusFile;                     // 应用目录状态文件
        QMap<QString, bool> launchedMap;        // 应用启动记录
        QMap<QString, bool> removedLaunchedMap; // desktop文件卸载记录
        QMap<QString, bool> uninstallMap;       // 记录应用将被卸载状态
    };

    AlRecorder(DFWatcher *_watcher, QObject *parent = nullptr);
    ~AlRecorder();

Q_SIGNALS:
    void Launched(const QString &file);
    void StatusSaved(const QString &root, const QString &file, bool ok);
    void ServiceRestarted();

private Q_SLOTS:
    void onDFChanged(const QString &filePath, uint32_t op);

public Q_SLOTS:
    QMap<QString, QStringList> GetNew();
    void MarkLaunched(const QString &filePath);
    void UninstallHints(const QStringList &desktopFiles);
    void WatchDirs(const QStringList &dataDirs);

private:
    void initSubRecoder(const QString &dirPath);
    void saveStatusFile(const QString &dirPath);

    QMap<QString,subRecorder> subRecoders;  // 记录不同应用目录的文件状态
    DFWatcher *watcher;
    QMutex mutex;
};

#endif // ALRECODER_H
