// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "compositorlink.h"

#include <QStringList>

// A compositor that isn't there. Writes down what the keyboard asks of it.
class FakeLink : public CompositorLink {
    Q_OBJECT

public:
    using CompositorLink::CompositorLink;

    QStringList asked;
    bool grabbed = false;

    void type(const QString &text) override
    {
        asked << QStringLiteral("type ") + text;
    }
    void deleteBefore(int bytes) override
    {
        asked << QStringLiteral("delete %1").arg(bytes);
    }
    void pressEnter() override
    {
        asked << QStringLiteral("enter");
    }
    void passOn(uint32_t code, bool pressed) override
    {
        asked << QStringLiteral("key %1 %2").arg(code).arg(pressed ? 1 : 0);
    }
    void setGrab(bool on) override
    {
        grabbed = on;
    }

    // What labwc sends when a text box takes focus, and when it goes.
    void focusBox(uint32_t purpose = 0, uint32_t hints = 0)
    {
        Q_EMIT activated();
        Q_EMIT done();
        Q_EMIT cursorChanged(false);
        Q_EMIT contentType(hints, purpose);
        Q_EMIT done();
        Q_EMIT cursorChanged(true);
    }
    void leaveBox()
    {
        Q_EMIT deactivated();
        Q_EMIT done();
    }

    // A key on a US keyboard.
    void press(uint32_t code)
    {
        if (!m_keymapLoaded) {
            m_keymapLoaded = m_keymap.load(Keymap::us());
        }
        Q_EMIT keyPressed(m_keymap.key(code));
    }
    void release(uint32_t code)
    {
        Q_EMIT keyReleased(code);
    }
    void tap(uint32_t code)
    {
        press(code);
        release(code);
    }

private:
    Keymap m_keymap;
    bool m_keymapLoaded = false;
};
