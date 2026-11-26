// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "layout.h"

#include <QFileSystemWatcher>
#include <QObject>
#include <QString>
#include <QtQml/qqmlregistration.h>

// This profile's settings for the keyboard, read again whenever they change:
// - ~/.config/hearth/keyboard.ini: layout=qwerty|alphabet|row|wheel
// - ~/.config/hearth/shell.ini, which the shell writes: the theme, the text size,
//   reduced motion and ignoring double presses, as ShellSettings reads them.
class KeyboardSettings : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("the program makes it")
    Q_PROPERTY(QString theme READ theme NOTIFY lookChanged)
    Q_PROPERTY(qreal textScale READ textScale NOTIFY lookChanged)
    Q_PROPERTY(bool reducedMotion READ reducedMotion NOTIFY lookChanged)

public:
    explicit KeyboardSettings(QObject *parent = nullptr);

    // QWERTY when the file says nothing, or something unknown.
    [[nodiscard]] LayoutKind layout() const
    {
        return m_layout;
    }
    // A built-in theme's name.
    [[nodiscard]] QString theme() const
    {
        return m_theme;
    }
    [[nodiscard]] qreal textScale() const
    {
        return m_textScale;
    }
    [[nodiscard]] bool reducedMotion() const
    {
        return m_reducedMotion;
    }
    [[nodiscard]] bool ignoresDoublePresses() const
    {
        return m_ignoreDoublePresses;
    }

Q_SIGNALS:
    void layoutChanged();
    void lookChanged();
    void ignoreDoublePressesChanged();

private:
    void read();
    void watch(const QString &path);

    QString m_keyboardPath;
    QString m_shellPath;
    QFileSystemWatcher m_watcher;
    LayoutKind m_layout = LayoutKind::Qwerty;
    QString m_theme;
    qreal m_textScale = 1.0;
    bool m_reducedMotion = false;
    bool m_ignoreDoublePresses = false;
};
