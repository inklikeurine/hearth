// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "keymap.h"
#include "layout.h"

#include <QElapsedTimer>
#include <QObject>
#include <QSet>
#include <QTimer>

// What a key means while the on-screen keyboard is shown. Keys are handled by
// what they are, not by which device sent them: arrows and Select work the
// keyboard, and the rest goes to the app as it came, typing included. (Typed
// text sent the input method's way can get lost in GTK apps when it comes fast.)
//
// A remote's OK and a keyboard's Enter are the same key: right after typing it's
// Enter, and after an arrow it presses the highlighted key.
//
// A held arrow repeats. Nothing else does.
//
// For hands that shake, a second press of the same arrow or Select can be dropped
// when it comes too soon to be meant, as the shell does for its own screens.
class KeyRules : public QObject {
    Q_OBJECT

public:
    explicit KeyRules(QObject *parent = nullptr);

    void press(const GrabbedKey &key);
    void release(uint32_t code);
    // Presses a second, and how many milliseconds before the first repeat.
    void setRepeat(int rate, int delay);
    void setIgnoreDoublePresses(bool ignore)
    {
        m_ignoreDoublePresses = ignore;
    }
    // Another text box, or none: what was typed before doesn't count.
    void reset();
    // A key on screen was pressed with the pointer.
    void pressedOnScreen();

Q_SIGNALS:
    void moved(Direction direction);
    void selected();
    // A key that types went to the app.
    void typedThrough(const QString &text);
    void backspace();
    void closed();
    // For the app, as it came.
    void passedOn(uint32_t code, bool pressed);

private:
    // A second press of the same button too soon after the last.
    [[nodiscard]] bool isDouble(uint32_t code);

    QTimer m_repeat;
    int m_rate = 25;
    int m_delay = 600;
    uint32_t m_repeating = 0;
    Direction m_direction = Direction::Up;
    bool m_typing = false;
    QSet<uint32_t> m_passedOn;
    bool m_ignoreDoublePresses = false;
    uint32_t m_lastButton = 0;
    QElapsedTimer m_sinceLastButton;
};
