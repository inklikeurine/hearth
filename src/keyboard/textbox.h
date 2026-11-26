// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fieldkind.h"

#include <QByteArray>
#include <QList>
#include <QString>

// The text box that has focus, from what the compositor sends: changes piece by
// piece, then "done".
//
// Only a box that describes itself counts, and apps differ in what they say:
// - Qt switches text input on for windows with no text box. A focused button gets
//   the same kind as an empty text box, but a cursor of no size (Hearth's failure
//   screen does this). When a Qt box takes focus later than its window, its cursor
//   is all that's heard of it: the compositor ignores the rest after that first,
//   empty switching on. And Qt doesn't switch off when focus leaves the box: the
//   cursor goes.
// - Firefox always sends the box's text, but a box a page focuses itself starts
//   with a cursor of no size.
// So: once a box has shown a cursor, it lasts as long as the cursor. Before that,
// its text is enough, and its kind is enough only when the compositor says
// nothing about cursors.
//
// The box's text is never kept, only how many bytes each character before the
// cursor takes, so Backspace can ask for exactly one of them.
class TextBox {
public:
    enum class Change {
        None,
        // A box took focus, or focus moved to another one.
        Focused,
        Unfocused,
    };

    void activate();
    void deactivate();
    // The text around the cursor; the cursor and the other end of the selection
    // are counted in bytes.
    void surroundingText(const QByteArray &text, qsizetype cursor, qsizetype anchor);
    void contentType(uint32_t hints, uint32_t purpose);
    [[nodiscard]] Change done();
    // Whether the app shows a cursor. Takes effect at once.
    [[nodiscard]] Change cursor(bool present);

    // An app's text box has focus.
    [[nodiscard]] bool isFocused() const
    {
        if (!m_active) {
            return false;
        }
        if (m_cursorSeen) {
            return m_cursor;
        }
        return m_sentText || (m_sentKind && !m_cursorKnown);
    }
    [[nodiscard]] ContentType content() const
    {
        return m_content;
    }
    [[nodiscard]] bool hasSelection() const
    {
        return m_selection;
    }

    // The keyboard typed this, or deleted one character: the app's own word on
    // it can come later than the next key.
    void typed(const QString &text);
    // How many bytes the character before the cursor takes, counting it deleted.
    int takeBefore();

private:
    bool m_active = false;
    // What the box said about itself since it was switched on.
    bool m_sentText = false;
    bool m_sentKind = false;
    bool m_cursor = false;
    // The compositor said something about the cursor.
    bool m_cursorKnown = false;
    // The box showed a cursor.
    bool m_cursorSeen = false;
    ContentType m_content;
    QList<quint8> m_sizesBefore;
    bool m_selection = false;

    struct Pending {
        bool activated = false;
        bool deactivated = false;
        bool surrounding = false;
        bool content = false;
        ContentType type;
        QList<quint8> sizesBefore;
        bool selection = false;
    } m_pending;
};
