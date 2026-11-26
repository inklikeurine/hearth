// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "fieldkind.h"

#include <QList>
#include <QObject>
#include <QRectF>
#include <QString>
#include <QtQml/qqmlregistration.h>

// What a key does. Screens know these as KeyAction.Shift, and so on.
namespace KeyAction {
Q_NAMESPACE
QML_ELEMENT

enum Kind {
    // Types the key's text.
    Type,
    Space,
    Backspace,
    Enter,
    Shift,
    // Show the symbols, or the letters again.
    Symbols,
    Letters,
    Close,
};
Q_ENUM_NS(Kind)

} // namespace KeyAction

struct Key {
    KeyAction::Kind action = KeyAction::Type;
    QString text;
    // In key widths and rows, from the top left.
    QRectF place;
};

// How the keys are arranged: the profile's choice.
enum class LayoutKind { Qwerty, Alphabet, Row, Wheel };

enum class Page { Letters, Symbols, Pad };

enum class Direction { Up, Down, Left, Right };

// One screenful of keys, and how the arrows move through them.
class Layout {
public:
    Layout() = default;
    Layout(LayoutKind kind, FieldKind field, Page page);

    // The page a box of this kind starts with.
    static Page firstPage(FieldKind field);

    [[nodiscard]] const QList<Key> &keys() const
    {
        return m_keys;
    }
    // All the keys' width and height, in keys.
    [[nodiscard]] QSizeF size() const
    {
        return m_size;
    }
    // How many key widths the screen shows. A row longer than this scrolls.
    [[nodiscard]] qreal visibleWidth() const
    {
        return m_visibleWidth;
    }
    // The key that starts highlighted.
    [[nodiscard]] int firstKey() const;
    // The first key that does this, or -1.
    [[nodiscard]] int find(KeyAction::Kind action) const;

    // Where an arrow leads. "column" is where up and down aim for, so that they
    // come back to the same key over a wide one; the caller keeps it.
    [[nodiscard]] int move(int from, Direction direction, qreal &column) const;

private:
    struct Cell {
        // A key that types. With no text, a gap.
        explicit Cell(const QString &types)
            : text(types)
        {
        }
        explicit Cell(KeyAction::Kind does, qreal keys = 1)
            : action(does)
            , width(keys)
        {
        }

        KeyAction::Kind action = KeyAction::Type;
        QString text;
        qreal width = 1;
    };
    using Cells = QList<Cell>;

    static Cells characters(const QString &text);
    static Cells extras(FieldKind field, Page page);
    void makeGrid(const QList<Cells> &rows, qreal visibleWidth);
    void makeRow(FieldKind field, Page page);
    void makeWheel(FieldKind field, Page page);
    [[nodiscard]] int moveSideways(int from, int step, qreal &column) const;
    [[nodiscard]] int moveBetweenRows(int from, int step, qreal column) const;
    [[nodiscard]] int moveOnWheel(int from, Direction direction) const;

    LayoutKind m_kind = LayoutKind::Qwerty;
    Page m_page = Page::Letters;
    QList<Key> m_keys;
    // The wheel's keys in their groups of four: each one's first key.
    QList<int> m_petals;
    QSizeF m_size;
    qreal m_visibleWidth = 1;
};
