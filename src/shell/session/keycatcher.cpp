// SPDX-License-Identifier: GPL-3.0-or-later
#include "keycatcher.h"

#include "labwcconfig.h"

#include <QGuiApplication>
#include <QKeyEvent>
#include <QWindow>

#include <array>
#include <xkbcommon/xkbcommon.h>

KeyCatcher::KeyCatcher(QObject *parent)
    : QObject(parent)
{
}

void KeyCatcher::setCatching(bool catching)
{
    if (catching == m_catching) {
        return;
    }
    m_catching = catching;
    if (m_catching) {
        qGuiApp->installEventFilter(this);
    } else {
        qGuiApp->removeEventFilter(this);
    }
    Q_EMIT catchingChanged();
}

bool KeyCatcher::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() != QEvent::KeyPress || !qobject_cast<QWindow *>(object)) {
        return QObject::eventFilter(object, event);
    }
    const auto *press = static_cast<QKeyEvent *>(event);
    if (press->isAutoRepeat()) {
        return true;
    }
    // On Wayland, Qt passes on the key's number in xkbcommon.
    std::array<char, 64> name { };
    xkb_keysym_get_name(press->nativeVirtualKey(), name.data(), name.size());
    const QString key = QString::fromLatin1(name.data());
    setCatching(false);
    if (LabwcConfig::mayBeUsed(key)) {
        Q_EMIT caught(key);
    } else {
        Q_EMIT refused();
    }
    return true;
}
