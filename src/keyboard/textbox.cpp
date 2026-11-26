// SPDX-License-Identifier: GPL-3.0-or-later
#include "textbox.h"

#include <algorithm>

namespace {

// How many bytes each character takes in UTF-8, one number for each.
QList<quint8> characterSizes(QByteArrayView text)
{
    QList<quint8> sizes;
    for (const char byte : text) {
        const bool continues = (static_cast<uchar>(byte) & 0xc0U) == 0x80U;
        if (continues && !sizes.isEmpty()) {
            ++sizes.back();
        } else {
            sizes.append(1);
        }
    }
    return sizes;
}

} // namespace

void TextBox::activate()
{
    // What follows describes the new box.
    m_pending = Pending();
    m_pending.activated = true;
}

void TextBox::deactivate()
{
    m_pending = Pending();
    m_pending.deactivated = true;
}

void TextBox::surroundingText(const QByteArray &text, qsizetype cursor, qsizetype anchor)
{
    const qsizetype end = std::clamp(cursor, qsizetype(0), text.size());
    m_pending.surrounding = true;
    m_pending.sizesBefore = characterSizes(QByteArrayView(text).first(end));
    m_pending.selection = cursor != anchor;
}

void TextBox::contentType(uint32_t hints, uint32_t purpose)
{
    m_pending.content = true;
    m_pending.type = { hints, purpose };
}

TextBox::Change TextBox::done()
{
    const Pending now = m_pending;
    m_pending = Pending();
    const bool wasFocused = isFocused();
    const FieldKind kindBefore = m_content.kind();

    if (now.activated || now.deactivated) {
        m_active = now.activated;
        m_sentText = false;
        m_sentKind = false;
        m_cursor = false;
        m_cursorKnown = false;
        m_cursorSeen = false;
        m_content = ContentType();
        m_sizesBefore.clear();
        m_selection = false;
    }
    if (m_active && now.surrounding) {
        m_sentText = true;
        m_sizesBefore = now.sizesBefore;
        m_selection = now.selection;
    }
    if (m_active && now.content) {
        m_sentKind = true;
        m_content = now.type;
    }
    // Focus moved inside a Qt window: all that changes is what the box is like.
    const bool anotherKind = m_content.kind() != kindBefore;
    if (isFocused() && (!wasFocused || now.activated || anotherKind)) {
        return Change::Focused;
    }
    return wasFocused && !isFocused() ? Change::Unfocused : Change::None;
}

TextBox::Change TextBox::cursor(bool present)
{
    const bool wasFocused = isFocused();
    m_cursor = present;
    m_cursorKnown = true;
    m_cursorSeen = m_cursorSeen || present;
    if (isFocused() == wasFocused) {
        return Change::None;
    }
    return wasFocused ? Change::Unfocused : Change::Focused;
}

void TextBox::typed(const QString &text)
{
    m_sizesBefore += characterSizes(text.toUtf8());
    m_selection = false;
}

int TextBox::takeBefore()
{
    if (m_sizesBefore.isEmpty()) {
        return 1;
    }
    const int size = m_sizesBefore.back();
    m_sizesBefore.removeLast();
    return size;
}
