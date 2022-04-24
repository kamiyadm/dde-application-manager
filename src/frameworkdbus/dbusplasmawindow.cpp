/*
 * This file was generated by qdbusxml2cpp-fix version 0.8
 * Command line was: qdbusxml2cpp-fix -c Window -p generated/window ../xml/Window.xml
 *
 * qdbusxml2cpp-fix is Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#include "dbusplasmawindow.h"

/*
 * Implementation of interface class __Window
 */

class __WindowPrivate
{
public:
   __WindowPrivate() = default;

    // begin member variables

public:
    QMap<QString, QDBusPendingCallWatcher *> m_processingCalls;
    QMap<QString, QList<QVariant>> m_waittingCalls;
};

PlasmaWindow::PlasmaWindow(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : DBusExtendedAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    , d_ptr(new __WindowPrivate)
{
    if (QMetaType::type("Rect") == QMetaType::UnknownType)
        registerRectMetaType();
}

PlasmaWindow::~PlasmaWindow()
{
    qDeleteAll(d_ptr->m_processingCalls.values());
    delete d_ptr;
}

void PlasmaWindow::CallQueued(const QString &callName, const QList<QVariant> &args)
{
    if (d_ptr->m_waittingCalls.contains(callName))
    {
        d_ptr->m_waittingCalls[callName] = args;
        return;
    }
    if (d_ptr->m_processingCalls.contains(callName))
    {
        d_ptr->m_waittingCalls.insert(callName, args);
    } else {
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(asyncCallWithArgumentList(callName, args));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &PlasmaWindow::onPendingCallFinished);
        d_ptr->m_processingCalls.insert(callName, watcher);
    }
}

void PlasmaWindow::onPendingCallFinished(QDBusPendingCallWatcher *w)
{
    w->deleteLater();
    const auto callName = d_ptr->m_processingCalls.key(w);
    Q_ASSERT(!callName.isEmpty());
    if (callName.isEmpty())
        return;
    d_ptr->m_processingCalls.remove(callName);
    if (!d_ptr->m_waittingCalls.contains(callName))
        return;
    const auto args = d_ptr->m_waittingCalls.take(callName);
    CallQueued(callName, args);
}
