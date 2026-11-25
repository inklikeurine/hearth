// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QDBusConnection>
#include <QObject>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

// How the box is connected, as NetworkManager says. It opens no connection of its
// own, so it knows about the cable or Wi-Fi, not about the internet beyond it.
class NetworkState : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(Kind kind READ kind NOTIFY changed)

public:
    enum Kind {
        Unknown, // NetworkManager isn't there to ask.
        None,
        Wired,
        Wifi
    };
    Q_ENUM(Kind)

    explicit NetworkState(QObject *parent = nullptr);
    NetworkState(const QDBusConnection &bus, QObject *parent);

    [[nodiscard]] Kind kind() const
    {
        return m_kind;
    }

Q_SIGNALS:
    void changed();

private Q_SLOTS:
    void propertiesChanged(
        const QString &interface, const QVariantMap &changed, const QStringList &invalidated);

private:
    void ask();
    void use(const QVariantMap &properties);

    QDBusConnection m_bus;
    Kind m_kind = Unknown;
    uint m_state = 0;
    QString m_type;
};
