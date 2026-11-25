// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

// Decides what a key labwc caught means right now. Keys are handled by what they
// are, not by which device sent them: Esc arrives here as Back.
class KeyRouter : public QObject {
    Q_OBJECT
    QML_ELEMENT
    // The shell's own window, the home screen, is in front.
    Q_PROPERTY(bool homeInFront MEMBER m_homeInFront NOTIFY changed)
    Q_PROPERTY(bool menuOpen MEMBER m_menuOpen NOTIFY changed)
    // A Hearth screen covers whatever is in front, such as a failure screen.
    Q_PROPERTY(bool screenOpen MEMBER m_screenOpen NOTIFY changed)
    Q_PROPERTY(bool keyboardShown MEMBER m_keyboardShown NOTIFY changed)
    Q_PROPERTY(bool textBoxFocused MEMBER m_textBoxFocused NOTIFY changed)

public:
    using QObject::QObject;

    Q_INVOKABLE void handle(const QString &command);

Q_SIGNALS:
    void changed();

    void goHome();
    void openMenu();
    void closeMenu();
    void closeKeyboard();
    // Delete the character before the cursor in the text box.
    void deleteBackward();
    // Back, for whichever of these is showing.
    void screenBack();
    void homeBack();
    void appBack();

private:
    void back();

    bool m_homeInFront = true;
    bool m_menuOpen = false;
    bool m_screenOpen = false;
    bool m_keyboardShown = false;
    bool m_textBoxFocused = false;
};
