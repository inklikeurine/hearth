// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QDBusConnection>
#include <QObject>
#include <QtQml/qqmlregistration.h>

// Sleep, restart and off, through logind.
class Power : public QObject {
    Q_OBJECT
    QML_ELEMENT

public:
    explicit Power(QObject *parent = nullptr);
    Power(const QDBusConnection &bus, QObject *parent);

    Q_INVOKABLE void sleep();
    Q_INVOKABLE void restart();
    Q_INVOKABLE void turnOff();

Q_SIGNALS:
    void failed();

private:
    void ask(const QString &method);

    QDBusConnection m_bus;
};
