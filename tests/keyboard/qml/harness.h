// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fakelink.h"
#include "keyboard.h"
#include "keyboardsettings.h"
#include "shelllink.h"

#include <QFile>
#include <QSettings>
#include <QTemporaryDir>
#include <QtQml/qqmlregistration.h>

// The keyboard, for the QML tests: with no compositor, no shell and its own
// settings.
class Harness : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(Keyboard *keyboard READ keyboard CONSTANT)
    Q_PROPERTY(QStringList asked READ asked)

public:
    [[nodiscard]] Keyboard *keyboard()
    {
        return &m_keyboard;
    }
    // What the keyboard asked of the compositor.
    [[nodiscard]] QStringList asked() const
    {
        return m_link.asked;
    }

    // The shell's look settings.
    Q_INVOKABLE void useLook(const QString &theme, int textPercent, bool reducedMotion)
    {
        QSettings file(QSettings::IniFormat, QSettings::UserScope, QStringLiteral("hearth"),
            QStringLiteral("shell"));
        file.setValue(QStringLiteral("look/theme"), theme);
        file.setValue(QStringLiteral("look/textPercent"), textPercent);
        file.setValue(QStringLiteral("look/reducedMotion"), reducedMotion);
    }
    Q_INVOKABLE void useLayout(const QString &name)
    {
        QSettings file(QSettings::IniFormat, QSettings::UserScope, QStringLiteral("hearth"),
            QStringLiteral("keyboard"));
        file.setValue(QStringLiteral("layout"), name);
    }
    Q_INVOKABLE [[nodiscard]] bool usesLayout(const QString &name) const
    {
        const QStringList names { QStringLiteral("qwerty"), QStringLiteral("alphabet"),
            QStringLiteral("row"), QStringLiteral("wheel") };
        return names.indexOf(name) == int(m_settings.layout());
    }
    Q_INVOKABLE void focusBox(int purpose)
    {
        m_link.leaveBox();
        m_link.focusBox(uint32_t(purpose));
    }
    Q_INVOKABLE void leaveBox()
    {
        m_link.leaveBox();
    }
    Q_INVOKABLE void forget()
    {
        m_link.asked.clear();
    }
    // Linux's number for a key.
    Q_INVOKABLE void tap(int code)
    {
        m_link.tap(uint32_t(code));
    }

private:
    static QString useOwnSettings(const QTemporaryDir &folder)
    {
        qputenv("XDG_CONFIG_HOME", QFile::encodeName(folder.path()));
        return { };
    }

    QTemporaryDir m_folder;
    FakeLink m_link;
    ShellLink m_shell { useOwnSettings(m_folder) };
    KeyboardSettings m_settings;
    Keyboard m_keyboard { &m_link, &m_settings, &m_shell };
};
