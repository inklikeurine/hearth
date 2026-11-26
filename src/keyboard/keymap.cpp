// SPDX-License-Identifier: GPL-3.0-or-later
#include "keymap.h"

#include <xkbcommon/xkbcommon.h>

#include <cstdlib>

namespace {

// XKB counts its key codes from 8.
constexpr uint32_t xkbOffset = 8;

} // namespace

void Keymap::Free::operator()(xkb_context *context) const
{
    xkb_context_unref(context);
}

void Keymap::Free::operator()(xkb_keymap *keymap) const
{
    xkb_keymap_unref(keymap);
}

void Keymap::Free::operator()(xkb_state *state) const
{
    xkb_state_unref(state);
}

Keymap::Keymap()
    : m_context(xkb_context_new(XKB_CONTEXT_NO_FLAGS))
{
}

bool Keymap::load(const QByteArray &text)
{
    m_state.reset();
    m_keymap.reset(xkb_keymap_new_from_string(
        m_context.get(), text.constData(), XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS));
    if (m_keymap) {
        m_state.reset(xkb_state_new(m_keymap.get()));
    }
    return m_state != nullptr;
}

void Keymap::setModifiers(uint32_t held, uint32_t latched, uint32_t locked, uint32_t group)
{
    if (m_state) {
        xkb_state_update_mask(m_state.get(), held, latched, locked, 0, 0, group);
    }
}

GrabbedKey Keymap::key(uint32_t code) const
{
    GrabbedKey key;
    key.code = code;
    if (!m_state) {
        return key;
    }
    key.symbol = xkb_state_key_get_one_sym(m_state.get(), code + xkbOffset);
    for (const char *modifier : { XKB_MOD_NAME_CTRL, XKB_MOD_NAME_ALT, XKB_MOD_NAME_LOGO }) {
        key.shortcut = key.shortcut
            || xkb_state_mod_name_is_active(m_state.get(), modifier, XKB_STATE_MODS_EFFECTIVE) > 0;
    }
    if (!key.shortcut) {
        const uint32_t character = xkb_state_key_get_utf32(m_state.get(), code + xkbOffset);
        // Enter, Tab and others come as control characters. They aren't text.
        if (character >= 0x20 && character != 0x7f) {
            key.text = QString::fromUcs4(reinterpret_cast<const char32_t *>(&character), 1);
        }
    }
    return key;
}

QByteArray Keymap::us()
{
    const Keymap maker;
    xkb_rule_names names { };
    names.layout = "us";
    const std::unique_ptr<xkb_keymap, Free> keymap(
        xkb_keymap_new_from_names(maker.m_context.get(), &names, XKB_KEYMAP_COMPILE_NO_FLAGS));
    if (!keymap) {
        return { };
    }
    char *text = xkb_keymap_get_as_string(keymap.get(), XKB_KEYMAP_FORMAT_TEXT_V1);
    QByteArray copy(text);
    // libxkbcommon's caller frees it.
    std::free(text);
    return copy;
}
