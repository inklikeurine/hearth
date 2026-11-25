// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "desktopentry.h"

#include <QHash>
#include <QObject>
#include <QProcess>
#include <QtQml/qqmlregistration.h>

// Starts and stops apps' programs. Which one is in front is AppSwitcher's job.
class AppLauncher : public QObject {
    Q_OBJECT
    QML_ELEMENT

public:
    explicit AppLauncher(QObject *parent = nullptr);
    ~AppLauncher() override;

    // Does nothing if it's already running.
    void start(const DesktopEntry &app);
    [[nodiscard]] QStringList running() const
    {
        return m_running.keys();
    }
    // Asks every app's program to end.
    void terminateAll();
    // Ends them at once; whatever they hadn't saved is lost.
    void killAll();

Q_SIGNALS:
    void started(const QString &appId);
    // It ended by itself with an error, or because it crashed.
    void finished(const QString &appId, bool withError, bool crashed);
    // The app's program couldn't be started at all.
    void failed(const QString &appId);

private:
    void signalAll(int signal);

    QHash<QString, QProcess *> m_running;
};
