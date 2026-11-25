// SPDX-License-Identifier: GPL-3.0-or-later
#include "power.h"

#include "buses.h"

#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

using namespace Qt::StringLiterals;

Power::Power(QObject *parent)
    : Power(Buses::system(), parent)
{
}

Power::Power(const QDBusConnection &bus, QObject *parent)
    : QObject(parent)
    , m_bus(bus)
{
}

void Power::sleep()
{
    ask(u"Suspend"_s);
}

void Power::restart()
{
    ask(u"Reboot"_s);
}

void Power::turnOff()
{
    ask(u"PowerOff"_s);
}

void Power::ask(const QString &method)
{
    if (!m_bus.isConnected()) {
        qWarning("no logind here: %s", qPrintable(method));
        Q_EMIT failed();
        return;
    }
    QDBusMessage call = QDBusMessage::createMethodCall(u"org.freedesktop.login1"_s,
        u"/org/freedesktop/login1"_s, u"org.freedesktop.login1.Manager"_s, method);
    // No password prompt: there's nothing to show one, and no keyboard.
    call << false;
    auto *watcher = new QDBusPendingCallWatcher(m_bus.asyncCall(call), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher, method] {
        const QDBusPendingReply<> reply = *watcher;
        if (reply.isError()) {
            qWarning("%s: %s", qPrintable(method), qPrintable(reply.error().message()));
            Q_EMIT failed();
        }
        watcher->deleteLater();
    });
}
