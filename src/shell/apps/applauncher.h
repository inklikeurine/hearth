// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "applist.h"

#include <QHash>
#include <QObject>
#include <QProcess>
#include <QtQml/qqmlregistration.h>

// Starts apps. Switching between running apps comes with the session (window
// switching); until then, an app that's already running isn't started twice.
class AppLauncher : public QObject {
    Q_OBJECT
    QML_ELEMENT

public:
    explicit AppLauncher(QObject *parent = nullptr);
    ~AppLauncher() override;

    Q_INVOKABLE void launch(AppList *apps, int index);

Q_SIGNALS:
    void started(const QString &appId);
    void finished(const QString &appId);
    // The app's program couldn't be started at all.
    void failed(const QString &appId, const QString &name);

private:
    QHash<QString, QProcess *> m_running;
};
