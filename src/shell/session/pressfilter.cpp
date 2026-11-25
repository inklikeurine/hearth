// SPDX-License-Identifier: GPL-3.0-or-later
#include "pressfilter.h"

#include <QDateTime>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QWindow>

PressFilter::PressFilter(QObject *parent)
    : QObject(parent)
{
}

void PressFilter::setActive(bool active)
{
    if (active == m_active) {
        return;
    }
    m_active = active;
    m_commands.clear();
    m_keys.clear();
    if (m_active) {
        qGuiApp->installEventFilter(this);
    } else {
        qGuiApp->removeEventFilter(this);
    }
    Q_EMIT activeChanged();
}

// Counts from the press before, let through or not: a shaking hand doesn't get
// through by pressing often enough.
bool PressFilter::soon(QHash<QString, qint64> &times, const QString &what) const
{
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    const qint64 before = times.value(what, 0);
    times.insert(what, now);
    return now - before < m_milliseconds;
}

bool PressFilter::allows(const QString &command)
{
    // Held, these repeat on purpose.
    if (!m_active || command == QLatin1String("backspace")
        || command.startsWith(QLatin1String("volume-"))) {
        return true;
    }
    return !soon(m_commands, command);
}

bool PressFilter::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress && qobject_cast<QWindow *>(object)) {
        const auto *press = static_cast<QKeyEvent *>(event);
        if (!press->isAutoRepeat() && soon(m_keys, QString::number(press->key()))) {
            return true;
        }
    }
    return QObject::eventFilter(object, event);
}
