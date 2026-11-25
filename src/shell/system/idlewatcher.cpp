// SPDX-License-Identifier: GPL-3.0-or-later
#include "idlewatcher.h"

#include <QCoreApplication>
#include <QEvent>

IdleWatcher::IdleWatcher(QObject *parent)
    : QObject(parent)
{
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, [this] { setIdle(true); });
    QCoreApplication::instance()->installEventFilter(this);
}

void IdleWatcher::setSeconds(int seconds)
{
    if (seconds != m_seconds) {
        m_seconds = seconds;
        Q_EMIT secondsChanged();
        restart();
    }
}

void IdleWatcher::setRunning(bool running)
{
    if (running != m_running) {
        m_running = running;
        Q_EMIT runningChanged();
        restart();
    }
}

void IdleWatcher::activity()
{
    restart();
}

void IdleWatcher::restart()
{
    setIdle(false);
    if (m_running && m_seconds > 0) {
        m_timer.start(m_seconds * 1000);
    } else {
        m_timer.stop();
    }
}

void IdleWatcher::setIdle(bool idle)
{
    if (idle != m_idle) {
        m_idle = idle;
        Q_EMIT idleChanged();
    }
}

bool IdleWatcher::eventFilter(QObject *object, QEvent *event)
{
    switch (event->type()) {
    case QEvent::KeyPress:
    case QEvent::MouseButtonPress:
    case QEvent::Wheel:
        // Not mouse movement: an air mouse on the sofa never lies quite still.
        restart();
        break;
    default:
        break;
    }
    return QObject::eventFilter(object, event);
}
