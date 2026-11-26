// SPDX-License-Identifier: GPL-3.0-or-later
#include "keyrules.h"

#include <xkbcommon/xkbcommon-keysyms.h>

#include <optional>
#include <utility>

namespace {

// As the shell's PressFilter has it.
constexpr qint64 tooSoon = 300;

std::optional<Direction> arrow(uint32_t symbol)
{
    switch (symbol) {
    case XKB_KEY_Up:
    case XKB_KEY_KP_Up:
        return Direction::Up;
    case XKB_KEY_Down:
    case XKB_KEY_KP_Down:
        return Direction::Down;
    case XKB_KEY_Left:
    case XKB_KEY_KP_Left:
        return Direction::Left;
    case XKB_KEY_Right:
    case XKB_KEY_KP_Right:
        return Direction::Right;
    default:
        return std::nullopt;
    }
}

// Shift, Ctrl, Caps Lock and so on. The app hears about them with the next key.
bool isModifier(uint32_t symbol)
{
    return (symbol >= XKB_KEY_Shift_L && symbol <= XKB_KEY_Hyper_R)
        || (symbol >= XKB_KEY_ISO_Lock && symbol <= XKB_KEY_ISO_Last_Group_Lock)
        || symbol == XKB_KEY_Mode_switch || symbol == XKB_KEY_Num_Lock;
}

} // namespace

KeyRules::KeyRules(QObject *parent)
    : QObject(parent)
{
    m_repeat.setTimerType(Qt::PreciseTimer);
    connect(&m_repeat, &QTimer::timeout, this, [this] {
        m_repeat.setInterval(1000 / m_rate);
        Q_EMIT moved(m_direction);
    });
}

void KeyRules::press(const GrabbedKey &key)
{
    if (isModifier(key.symbol)) {
        return;
    }
    const std::optional<Direction> direction = arrow(key.symbol);
    if (direction && !key.shortcut) {
        if (isDouble(key.code)) {
            return;
        }
        m_typing = false;
        m_direction = *direction;
        m_repeating = key.code;
        if (m_rate > 0) {
            m_repeat.start(m_delay);
        }
        Q_EMIT moved(*direction);
        return;
    }
    switch (key.shortcut ? 0 : key.symbol) {
    case XKB_KEY_Return:
    case XKB_KEY_KP_Enter:
        if (m_typing) {
            break;
        }
        [[fallthrough]];
    case XKB_KEY_XF86OK:
    case XKB_KEY_XF86Select:
    case XKB_KEY_Select:
        if (!isDouble(key.code)) {
            m_typing = false;
            Q_EMIT selected();
        }
        return;
    // In a Hearth session labwc takes these first.
    case XKB_KEY_BackSpace:
        Q_EMIT backspace();
        return;
    case XKB_KEY_Escape:
    case XKB_KEY_XF86Back:
        Q_EMIT closed();
        return;
    default:
        break;
    }
    // Typing, or a shortcut, comes from someone at a keyboard. Volume keys don't.
    m_typing = m_typing || key.shortcut || !key.text.isEmpty();
    m_passedOn.insert(key.code);
    Q_EMIT passedOn(key.code, true);
    if (!key.text.isEmpty()) {
        Q_EMIT typedThrough(key.text);
    }
}

bool KeyRules::isDouble(uint32_t code)
{
    const bool same = code == m_lastButton && m_sinceLastButton.isValid()
        && m_sinceLastButton.elapsed() < tooSoon;
    m_lastButton = code;
    m_sinceLastButton.start();
    return m_ignoreDoublePresses && same;
}

void KeyRules::release(uint32_t code)
{
    if (code == m_repeating) {
        m_repeat.stop();
        m_repeating = 0;
    }
    if (m_passedOn.remove(code)) {
        Q_EMIT passedOn(code, false);
    }
}

void KeyRules::setRepeat(int rate, int delay)
{
    m_rate = rate;
    m_delay = delay;
    if (m_rate <= 0) {
        m_repeat.stop();
    }
}

void KeyRules::reset()
{
    m_repeat.stop();
    m_repeating = 0;
    m_typing = false;
    const QSet<uint32_t> held = std::exchange(m_passedOn, { });
    for (const uint32_t code : held) {
        Q_EMIT passedOn(code, false);
    }
}

void KeyRules::pressedOnScreen()
{
    m_typing = false;
}
