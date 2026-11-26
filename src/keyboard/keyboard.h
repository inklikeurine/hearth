// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "keyboardmodel.h"
#include "keyboardsettings.h"
#include "keyrules.h"
#include "textbox.h"

#include <QObject>
#include <QSet>
#include <QTimer>
#include <QtQml/qqmlregistration.h>

class CompositorLink;
class ShellLink;

// The on-screen keyboard: shows up for a text box, types into it, and goes when
// the box loses focus. Closed by the person, it stays closed until another box
// takes focus.
//
// What's typed goes to the text box and nowhere else. It's never kept or logged.
class Keyboard : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("the program makes it")
    Q_PROPERTY(bool shown READ isShown NOTIFY shownChanged)
    Q_PROPERTY(bool sensitive READ isSensitive NOTIFY boxChanged)
    Q_PROPERTY(KeyboardModel *keys READ keys CONSTANT)
    Q_PROPERTY(KeyboardSettings *settings READ settings CONSTANT)

public:
    Keyboard(CompositorLink *link, KeyboardSettings *settings, ShellLink *shell,
        QObject *parent = nullptr);

    [[nodiscard]] bool isShown() const
    {
        return m_shown;
    }
    // The box takes a password or a PIN.
    [[nodiscard]] bool isSensitive() const
    {
        return m_box.content().isSensitive();
    }
    [[nodiscard]] KeyboardModel *keys()
    {
        return &m_keys;
    }
    [[nodiscard]] KeyboardSettings *settings() const
    {
        return m_settings;
    }

    // Closed by the person.
    void close();
    void backspace();

Q_SIGNALS:
    void shownChanged();
    void boxChanged();

private:
    void follow(TextBox::Change change);
    void settle();
    void openKeys();
    void setShown(bool shown);
    void type(const QString &text);
    void press(const GrabbedKey &key);
    void release(uint32_t code);
    void letGoOfKeys();

    CompositorLink *m_link;
    KeyboardSettings *m_settings;
    ShellLink *m_shell;
    TextBox m_box;
    KeyRules m_rules;
    KeyboardModel m_keys;
    bool m_shown = false;
    // A box took focus, and the keyboard hasn't answered it yet.
    bool m_newBox = false;
    QTimer m_settle;
    // Grabbed keys still held down.
    QSet<uint32_t> m_held;
    QTimer m_letGo;
};
