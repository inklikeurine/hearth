// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "applauncher.h"
#include "applist.h"
#include "windows.h"

#include <QHash>
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QtQml/qqmlregistration.h>

#include <optional>

// One full-screen app at a time: opens apps, brings the home screen or the
// running app to the front, and closes one app before opening the next.
//
// It never ends an app that still has a window unless it's told to with
// forceClose(): that app may be holding work that isn't saved.
class AppSwitcher : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(Windows *windows MEMBER m_windows WRITE setWindows NOTIFY setUp)
    Q_PROPERTY(AppList *apps MEMBER m_apps NOTIFY setUp)
    Q_PROPERTY(AppLauncher *launcher MEMBER m_launcher WRITE setLauncher NOTIFY setUp)
    // How long an app gets to close its windows, and then to end.
    Q_PROPERTY(int closeWait MEMBER m_closeWait NOTIFY setUp)
    Q_PROPERTY(bool homeInFront READ homeInFront NOTIFY changed)
    // The app that's open, in front or not. Empty when there's none.
    Q_PROPERTY(QString runningId READ runningId NOTIFY changed)
    Q_PROPERTY(QString runningName READ runningName NOTIFY changed)
    // The name of an app that's been chosen and has no window yet.
    Q_PROPERTY(QString openingName READ openingName NOTIFY changed)

public:
    explicit AppSwitcher(QObject *parent = nullptr);

    void setWindows(Windows *windows);
    void setLauncher(AppLauncher *launcher);

    [[nodiscard]] bool homeInFront() const
    {
        return m_homeInFront;
    }
    [[nodiscard]] QString runningId() const
    {
        return m_runningId;
    }
    [[nodiscard]] QString runningName() const
    {
        return m_runningName;
    }
    [[nodiscard]] QString openingName() const
    {
        return m_openingName;
    }

    // Opens an app, or brings it back if it's the one running.
    Q_INVOKABLE void open(int index);
    Q_INVOKABLE void goHome();
    // Brings the running app back to the front.
    Q_INVOKABLE void returnToApp();
    // Asks the running app to close.
    Q_INVOKABLE void closeApp();
    // Ends the running app; whatever it hadn't saved is lost.
    Q_INVOKABLE void forceClose();

Q_SIGNALS:
    void setUp();
    void changed();
    // The home screen came to the front.
    void homeShown();

    // The app's program couldn't be started.
    void couldNotStart(const QString &name);
    // The app ended with an error before it showed anything.
    void endedEarly(const QString &name);
    // The app crashed while it was open.
    void crashed(const QString &name);
    // The app was asked to close and still has a window.
    void stillOpen(const QString &name);

private:
    void update();
    void updateFront(const QList<WindowInfo> &windows);
    void updateRunning(const QList<WindowInfo> &windows);
    void makeFullscreen(const QList<WindowInfo> &windows);
    void start(const DesktopEntry &app);
    void askToClose();
    void closeTimedOut();
    void appEnded(const QString &id, bool withError, bool crashed);
    void launchFailed(const QString &id);
    void setRunning(const QString &id, const QString &name, const QString &opening);
    [[nodiscard]] QList<WindowInfo> appWindows() const;
    [[nodiscard]] const DesktopEntry *appFor(const WindowInfo &window) const;
    [[nodiscard]] QString nameOf(const QString &id) const;

    QPointer<Windows> m_windows;
    QPointer<AppList> m_apps;
    QPointer<AppLauncher> m_launcher;
    int m_closeWait = 5000;

    bool m_homeInFront = true;
    QString m_runningId;
    QString m_runningName;
    QString m_openingName;
    // Opened by us, and whether it has shown a window since.
    std::optional<DesktopEntry> m_launched;
    bool m_hadWindow = false;
    // Waits for the running app to close.
    std::optional<DesktopEntry> m_next;
    bool m_closing = false;
    bool m_terminated = false;
    QTimer m_closeTimer;
    // How often each window was made full screen.
    QHash<int, int> m_fullscreened;
};
