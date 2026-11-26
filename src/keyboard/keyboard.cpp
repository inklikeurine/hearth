// SPDX-License-Identifier: GPL-3.0-or-later
#include "keyboard.h"

#include "compositorlink.h"
#include "shelllink.h"

#include <chrono>

using namespace Qt::StringLiterals;
using namespace std::chrono_literals;

Keyboard::Keyboard(
    CompositorLink *link, KeyboardSettings *settings, ShellLink *shell, QObject *parent)
    : QObject(parent)
    , m_link(link)
    , m_settings(settings)
    , m_shell(shell)
{
    connect(link, &CompositorLink::activated, this, [this] { m_box.activate(); });
    connect(link, &CompositorLink::deactivated, this, [this] { m_box.deactivate(); });
    connect(link, &CompositorLink::surroundingText, this,
        [this](const QByteArray &text, qsizetype cursor, qsizetype anchor) {
            m_box.surroundingText(text, cursor, anchor);
        });
    connect(link, &CompositorLink::contentType, this,
        [this](uint32_t hints, uint32_t purpose) { m_box.contentType(hints, purpose); });
    connect(link, &CompositorLink::done, this, [this] { follow(m_box.done()); });
    connect(link, &CompositorLink::cursorChanged, this,
        [this](bool present) { follow(m_box.cursor(present)); });
    connect(link, &CompositorLink::keyPressed, this, &Keyboard::press);
    connect(link, &CompositorLink::keyReleased, this, &Keyboard::release);
    connect(link, &CompositorLink::repeatChanged, &m_rules, &KeyRules::setRepeat);

    connect(&m_rules, &KeyRules::moved, &m_keys, &KeyboardModel::move);
    connect(&m_rules, &KeyRules::selected, &m_keys, &KeyboardModel::press);
    connect(&m_rules, &KeyRules::typedThrough, this, [this](const QString &text) {
        if (m_box.isFocused()) {
            m_box.typed(text);
        }
    });
    connect(&m_rules, &KeyRules::backspace, this, &Keyboard::backspace);
    connect(&m_rules, &KeyRules::closed, this, &Keyboard::close);
    connect(&m_rules, &KeyRules::passedOn, link, &CompositorLink::passOn);

    connect(&m_keys, &KeyboardModel::typed, this, &Keyboard::type);
    connect(&m_keys, &KeyboardModel::backspace, this, &Keyboard::backspace);
    connect(&m_keys, &KeyboardModel::enter, link, &CompositorLink::pressEnter);
    connect(&m_keys, &KeyboardModel::close, this, &Keyboard::close);
    connect(&m_keys, &KeyboardModel::pressed, &m_rules, &KeyRules::pressedOnScreen);

    connect(settings, &KeyboardSettings::layoutChanged, this, &Keyboard::openKeys);
    const auto ignoreDoublePresses
        = [this] { m_rules.setIgnoreDoublePresses(m_settings->ignoresDoublePresses()); };
    connect(settings, &KeyboardSettings::ignoreDoublePressesChanged, this, ignoreDoublePresses);
    ignoreDoublePresses();
    connect(shell, &ShellLink::hideAsked, this, &Keyboard::close);
    connect(shell, &ShellLink::backspaceAsked, this, &Keyboard::backspace);

    m_settle.setSingleShot(true);
    connect(&m_settle, &QTimer::timeout, this, &Keyboard::settle);
    // A key that never comes up mustn't keep the keyboard's hold on the rest.
    m_letGo.setSingleShot(true);
    m_letGo.setInterval(2s);
    connect(&m_letGo, &QTimer::timeout, this, &Keyboard::letGoOfKeys);
}

// Focus moving between two boxes comes as leaving the one and then entering the
// other, and a box's kind comes a little after the box.
void Keyboard::follow(TextBox::Change change)
{
    if (change != TextBox::Change::None) {
        m_newBox = change == TextBox::Change::Focused;
        m_settle.start(m_newBox ? 0ms : 50ms);
    }
}

void Keyboard::settle()
{
    if (m_newBox) {
        m_newBox = false;
        m_rules.reset();
        openKeys();
    }
    setShown(m_box.isFocused());
    Q_EMIT boxChanged();
    m_shell->setState(m_shown, m_box.isFocused());
}

void Keyboard::openKeys()
{
    m_keys.open(m_settings->layout(), m_box.content().kind());
}

void Keyboard::setShown(bool shown)
{
    if (shown) {
        m_letGo.stop();
        m_link->setGrab(true);
    } else if (m_shown) {
        m_rules.reset();
        // The app mustn't see a key come up that it never saw go down.
        if (m_held.isEmpty()) {
            letGoOfKeys();
        } else {
            m_letGo.start();
        }
    }
    if (shown != m_shown) {
        m_shown = shown;
        Q_EMIT shownChanged();
    }
}

void Keyboard::letGoOfKeys()
{
    m_letGo.stop();
    m_held.clear();
    m_link->setGrab(false);
}

void Keyboard::close()
{
    setShown(false);
    m_shell->setState(m_shown, m_box.isFocused());
}

void Keyboard::type(const QString &text)
{
    if (m_box.isFocused()) {
        m_link->type(text);
        m_box.typed(text);
    }
}

void Keyboard::backspace()
{
    if (!m_box.isFocused()) {
        return;
    }
    if (m_box.content().isTerminal()) {
        // Terminals can't delete what they've shown. This is what their own
        // Backspace sends to the program in them.
        m_link->type(u"\x7f"_s);
        return;
    }
    if (m_box.hasSelection()) {
        // Typing replaces what's selected in every app. Deleting next to a
        // selection means something else in each.
        type(u" "_s);
    }
    m_link->deleteBefore(m_box.takeBefore());
}

void Keyboard::press(const GrabbedKey &key)
{
    m_held.insert(key.code);
    if (m_shown) {
        m_rules.press(key);
    }
}

void Keyboard::release(uint32_t code)
{
    m_rules.release(code);
    m_held.remove(code);
    if (!m_shown && m_held.isEmpty()) {
        letGoOfKeys();
    }
}
