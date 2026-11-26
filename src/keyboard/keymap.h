// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QByteArray>
#include <QString>

#include <cstdint>
#include <memory>

struct xkb_context;
struct xkb_keymap;
struct xkb_state;

// A key from a real keyboard or a remote.
struct GrabbedKey {
    // Linux's number for the key.
    uint32_t code = 0;
    // XKB's number for what the key is: an arrow, a letter.
    uint32_t symbol = 0;
    // What it types; nothing for keys that don't.
    QString text;
    // Ctrl, Alt or the logo key is held with it: a shortcut, not typing.
    bool shortcut = false;
};

// What a keyboard's keys mean, through libxkbcommon.
class Keymap {
public:
    Keymap();

    // Takes a keymap as text, as the compositor sends it. False if it's no good.
    bool load(const QByteArray &text);
    void setModifiers(uint32_t held, uint32_t latched, uint32_t locked, uint32_t group);
    [[nodiscard]] GrabbedKey key(uint32_t code) const;

    // A US keyboard's keymap, as text.
    static QByteArray us();

private:
    struct Free {
        void operator()(xkb_context *context) const;
        void operator()(xkb_keymap *keymap) const;
        void operator()(xkb_state *state) const;
    };
    std::unique_ptr<xkb_context, Free> m_context;
    std::unique_ptr<xkb_keymap, Free> m_keymap;
    std::unique_ptr<xkb_state, Free> m_state;
};
