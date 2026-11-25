// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QProcess>
#include <QStringList>
#include <QtQml/qqmlregistration.h>

// The volume of the sound output in use, through WirePlumber's wpctl.
class Volume : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int percent READ percent NOTIFY changed)
    Q_PROPERTY(bool muted READ muted NOTIFY changed)

public:
    explicit Volume(QObject *parent = nullptr);

    [[nodiscard]] int percent() const
    {
        return m_percent;
    }
    [[nodiscard]] bool muted() const
    {
        return m_muted;
    }

    Q_INVOKABLE void up();
    Q_INVOKABLE void down();
    Q_INVOKABLE void toggleMute();

Q_SIGNALS:
    void changed();
    // A button was pressed and did what it should; show the volume.
    void shown();
    // There's no sound output, or wpctl doesn't answer.
    void failed();

private:
    void run(const QStringList &arguments);
    void read();

    // What's asked while wpctl is busy waits, at most one press.
    QList<QStringList> m_waiting;
    QProcess m_process;
    bool m_reading = false;
    int m_percent = 0;
    bool m_muted = false;
};
