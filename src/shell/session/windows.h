// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QList>
#include <QObject>
#include <QString>

// One window open in the session.
struct WindowInfo {
    int id = 0;
    QString appId;
    QString title;
    bool active = false;
    bool minimized = false;
    bool fullscreen = false;
    // An app's dialog: it belongs to another window.
    bool hasParent = false;
};

// The session's windows. WaylandWindows is the real one; tests use their own.
class Windows : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

    [[nodiscard]] virtual QList<WindowInfo> windows() const = 0;
    virtual void bringToFront(int id) = 0;
    virtual void minimize(int id) = 0;
    virtual void makeFullscreen(int id) = 0;
    // Asks the app to close it. The app may refuse, or ask about unsaved work.
    virtual void close(int id) = 0;

Q_SIGNALS:
    void changed();
};
