// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QHash>
#include <QObject>
#include <QtQml/qqmlregistration.h>

// Drops a second press of the same button that comes too soon to be meant, for
// people whose hands shake. Covers Hearth's own screens and the buttons labwc
// catches, not what apps get.
class PressFilter : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(int milliseconds MEMBER m_milliseconds NOTIFY millisecondsChanged)

public:
    explicit PressFilter(QObject *parent = nullptr);

    [[nodiscard]] bool active() const
    {
        return m_active;
    }
    void setActive(bool active);
    // For a button labwc caught.
    Q_INVOKABLE bool allows(const QString &command);
    bool eventFilter(QObject *object, QEvent *event) override;

Q_SIGNALS:
    void activeChanged();
    void millisecondsChanged();

private:
    bool soon(QHash<QString, qint64> &times, const QString &what) const;

    QHash<QString, qint64> m_commands;
    QHash<QString, qint64> m_keys;
    int m_milliseconds = 300;
    bool m_active = false;
};
