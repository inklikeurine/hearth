// SPDX-License-Identifier: GPL-3.0-or-later
#include "keyboardsettings.h"

#include <QFileInfo>
#include <QHash>
#include <QSettings>

#include <algorithm>

using namespace Qt::StringLiterals;

namespace {

QString settingsFile(const QString &name)
{
    return QSettings(QSettings::IniFormat, QSettings::UserScope, u"hearth"_s, name).fileName();
}

} // namespace

KeyboardSettings::KeyboardSettings(QObject *parent)
    : QObject(parent)
    , m_keyboardPath(settingsFile(u"keyboard"_s))
    , m_shellPath(settingsFile(u"shell"_s))
{
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &KeyboardSettings::read);
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &KeyboardSettings::read);
    read();
}

void KeyboardSettings::read()
{
    // Before reading, so that no change falls in between.
    watch(m_keyboardPath);
    watch(m_shellPath);

    static const QHash<QString, LayoutKind> names {
        { u"qwerty"_s, LayoutKind::Qwerty },
        { u"alphabet"_s, LayoutKind::Alphabet },
        { u"row"_s, LayoutKind::Row },
        { u"wheel"_s, LayoutKind::Wheel },
    };
    const QSettings keyboard(m_keyboardPath, QSettings::IniFormat);
    const LayoutKind layout
        = names.value(keyboard.value(u"layout"_s).toString(), LayoutKind::Qwerty);
    if (layout != m_layout) {
        m_layout = layout;
        Q_EMIT layoutChanged();
    }

    // The same defaults and limits as ShellSettings.
    const QSettings shell(m_shellPath, QSettings::IniFormat);
    const QString theme = shell.value(u"look/theme"_s, u"dark"_s).toString();
    const qreal textScale
        = std::clamp(shell.value(u"look/textPercent"_s, 100).toInt(), 100, 150) / 100.0;
    const bool reducedMotion = shell.value(u"look/reducedMotion"_s, false).toBool();
    if (theme != m_theme || textScale != m_textScale || reducedMotion != m_reducedMotion) {
        m_theme = theme;
        m_textScale = textScale;
        m_reducedMotion = reducedMotion;
        Q_EMIT lookChanged();
    }
    const bool ignore = shell.value(u"keys/ignoreDoublePresses"_s, false).toBool();
    if (ignore != m_ignoreDoublePresses) {
        m_ignoreDoublePresses = ignore;
        Q_EMIT ignoreDoublePressesChanged();
    }
}

void KeyboardSettings::watch(const QString &path)
{
    // A file that's replaced by a new one drops out of the watch. Its folder, or
    // the nearest one that exists, says when it's back.
    QFileInfo nearest(path);
    if (nearest.exists()) {
        m_watcher.addPath(path);
    }
    do {
        nearest = QFileInfo(nearest.absolutePath());
    } while (!nearest.isDir() && !nearest.isRoot());
    m_watcher.addPath(nearest.absoluteFilePath());
}
