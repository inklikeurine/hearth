// SPDX-License-Identifier: GPL-3.0-or-later
#include "networkstate.h"

#include "buses.h"

#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

using namespace Qt::StringLiterals;

namespace {

constexpr auto service = "org.freedesktop.NetworkManager"_L1;
constexpr auto path = "/org/freedesktop/NetworkManager"_L1;
constexpr auto properties = "org.freedesktop.DBus.Properties"_L1;
// NMState: connected to a network, without or with more beyond it.
constexpr uint connectedLocal = 50;

} // namespace

NetworkState::NetworkState(QObject *parent)
    : NetworkState(Buses::system(), parent)
{
}

NetworkState::NetworkState(const QDBusConnection &bus, QObject *parent)
    : QObject(parent)
    , m_bus(bus)
{
    if (!m_bus.isConnected()) {
        return;
    }
    const QString sender = m_bus.interface() ? QString(service) : QString();
    m_bus.connect(sender, path, properties, u"PropertiesChanged"_s, this,
        SLOT(propertiesChanged(QString, QVariantMap, QStringList)));
    ask();
}

void NetworkState::ask()
{
    QDBusMessage call = QDBusMessage::createMethodCall(service, path, properties, u"GetAll"_s);
    call << QString(service);
    auto *watcher = new QDBusPendingCallWatcher(m_bus.asyncCall(call), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        const QDBusPendingReply<QVariantMap> reply = *watcher;
        if (!reply.isError()) {
            use(reply.value());
        }
        watcher->deleteLater();
    });
}

void NetworkState::propertiesChanged(
    const QString &interface, const QVariantMap &changed, const QStringList &)
{
    if (interface == service) {
        use(changed);
    }
}

void NetworkState::use(const QVariantMap &properties)
{
    m_state = properties.value(u"State"_s, m_state).toUInt();
    m_type = properties.value(u"PrimaryConnectionType"_s, m_type).toString();
    Kind kind = None;
    if (m_state >= connectedLocal) {
        kind = m_type == "802-11-wireless"_L1 ? Wifi : Wired;
    }
    if (kind != m_kind) {
        m_kind = kind;
        Q_EMIT changed();
    }
}
