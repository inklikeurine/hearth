// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QDBusConnection>
#include <QObject>
#include <QtQml/qqmlregistration.h>

// Play and pause for whichever app is playing, through MPRIS
// (https://specifications.freedesktop.org/mpris-spec/latest/). It looks when
// asked to: nothing runs while the menu is closed.
class MediaPlayers : public QObject {
    Q_OBJECT
    QML_ELEMENT
    // There's a player that can be played or paused.
    Q_PROPERTY(bool available READ available NOTIFY changed)
    Q_PROPERTY(bool playing READ playing NOTIFY changed)

public:
    explicit MediaPlayers(QObject *parent = nullptr);
    MediaPlayers(const QDBusConnection &bus, QObject *parent);

    [[nodiscard]] bool available() const
    {
        return !m_player.isEmpty();
    }
    [[nodiscard]] bool playing() const
    {
        return m_playing;
    }

    Q_INVOKABLE void look();
    Q_INVOKABLE void playPause();

Q_SIGNALS:
    void changed();

private:
    void ask(const QString &player);
    void use(const QString &player, bool playing);

    QDBusConnection m_bus;
    QString m_player;
    bool m_playing = false;
};
