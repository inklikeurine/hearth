// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QTimer>
#include <QtQml/qqmlregistration.h>

// Says when nobody has pressed anything on Hearth's own screens for a while. It
// doesn't see what apps get; keys labwc catches are passed on with activity().
class IdleWatcher : public QObject {
    Q_OBJECT
    QML_ELEMENT
    // In seconds; 0 is never.
    Q_PROPERTY(int seconds READ seconds WRITE setSeconds NOTIFY secondsChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool idle READ idle NOTIFY idleChanged)

public:
    explicit IdleWatcher(QObject *parent = nullptr);

    [[nodiscard]] int seconds() const
    {
        return m_seconds;
    }
    void setSeconds(int seconds);
    [[nodiscard]] bool running() const
    {
        return m_running;
    }
    void setRunning(bool running);
    [[nodiscard]] bool idle() const
    {
        return m_idle;
    }

    Q_INVOKABLE void activity();
    bool eventFilter(QObject *object, QEvent *event) override;

Q_SIGNALS:
    void secondsChanged();
    void runningChanged();
    void idleChanged();

private:
    void restart();
    void setIdle(bool idle);

    QTimer m_timer;
    int m_seconds = 0;
    bool m_running = true;
    bool m_idle = false;
};
