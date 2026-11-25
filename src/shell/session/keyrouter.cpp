// SPDX-License-Identifier: GPL-3.0-or-later
#include "keyrouter.h"

using namespace Qt::StringLiterals;

void KeyRouter::handle(const QString &command)
{
    if (command == u"home"_s) {
        Q_EMIT goHome();
    } else if (command == u"menu"_s) {
        if (m_menuOpen) {
            Q_EMIT closeMenu();
        } else {
            Q_EMIT openMenu();
        }
    } else if (command == u"back"_s) {
        back();
    } else if (command == u"backspace"_s) {
        // The menu has no text boxes, and takes the keys from any behind it.
        if (m_textBoxFocused && !m_menuOpen) {
            Q_EMIT deleteBackward();
        } else {
            back();
        }
    }
}

void KeyRouter::back()
{
    if (m_menuOpen) {
        Q_EMIT closeMenu();
    } else if (m_keyboardShown) {
        Q_EMIT closeKeyboard();
    } else if (m_screenOpen) {
        Q_EMIT screenBack();
    } else if (m_homeInFront) {
        Q_EMIT homeBack();
    } else {
        Q_EMIT appBack();
    }
}
