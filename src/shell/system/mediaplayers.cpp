// SPDX-License-Identifier: GPL-3.0-or-later
#include "mediaplayers.h"

#include "buses.h"

#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QRegularExpression>
#include <QVariantMap>

#include <algorithm>

using namespace Qt::StringLiterals;

namespace {

constexpr auto path = "/org/mpris/MediaPlayer2"_L1;
constexpr auto interface = "org.mpris.MediaPlayer2.Player"_L1;
// A player that doesn't answer mustn't hold the menu up.
constexpr int waitMs = 1000;
constexpr qsizetype mostPlayers = 20;

} // namespace

MediaPlayers::MediaPlayers(QObject *parent)
    : MediaPlayers(Buses::session(), parent)
{
}

MediaPlayers::MediaPlayers(const QDBusConnection &bus, QObject *parent)
    : QObject(parent)
    , m_bus(bus)
{
}

void MediaPlayers::look()
{
    if (!m_player.isEmpty()) {
        m_player.clear();
        m_playing = false;
        Q_EMIT changed();
    }
    if (!m_bus.isConnected()) {
        return;
    }
    const QDBusMessage call = QDBusMessage::createMethodCall(u"org.freedesktop.DBus"_s,
        u"/org/freedesktop/DBus"_s, u"org.freedesktop.DBus"_s, u"ListNames"_s);
    auto *watcher = new QDBusPendingCallWatcher(m_bus.asyncCall(call, waitMs), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        const QDBusPendingReply<QStringList> reply = *watcher;
        watcher->deleteLater();
        if (reply.isError()) {
            return;
        }
        const QStringList players
            = reply.value().filter(QRegularExpression(u"^org\\.mpris\\.MediaPlayer2\\."_s));
        for (const auto shown = players.first(std::min(players.size(), mostPlayers));
            const QString &player : shown) {
            ask(player);
        }
    });
}

void MediaPlayers::ask(const QString &player)
{
    QDBusMessage call = QDBusMessage::createMethodCall(
        player, path, u"org.freedesktop.DBus.Properties"_s, u"GetAll"_s);
    call << QString(interface);
    // Owned by this, and deleted when the answer comes; the checker can't follow it.
    // NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
    auto *watcher = new QDBusPendingCallWatcher(m_bus.asyncCall(call, waitMs), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher, player] {
        const QDBusPendingReply<QVariantMap> reply = *watcher;
        watcher->deleteLater();
        if (reply.isError()) {
            return;
        }
        const QString status = reply.value().value(u"PlaybackStatus"_s).toString();
        if (status == "Playing"_L1 || status == "Paused"_L1) {
            use(player, status == "Playing"_L1);
        }
    });
}
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)

void MediaPlayers::use(const QString &player, bool playing)
{
    // The one that's playing wins over ones that are paused.
    if (m_player.isEmpty() || (playing && !m_playing)) {
        m_player = player;
        m_playing = playing;
        Q_EMIT changed();
    }
}

void MediaPlayers::playPause()
{
    if (m_player.isEmpty() || !m_bus.isConnected()) {
        return;
    }
    m_bus.call(
        QDBusMessage::createMethodCall(m_player, path, interface, u"PlayPause"_s), QDBus::NoBlock);
    // What it's asked for; the next look says what happened.
    m_playing = !m_playing;
    Q_EMIT changed();
}
