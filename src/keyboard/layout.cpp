// SPDX-License-Identifier: GPL-3.0-or-later
#include "layout.h"

#include <algorithm>
#include <cmath>

using namespace Qt::StringLiterals;

namespace {

constexpr qreal gridWidth = 11;
constexpr qreal rowWidth = 18;
constexpr int petalSize = 4;
// The gap between the wheel's petals, in keys.
constexpr qreal petalGap = 0.5;

// The keys that differ between kinds of text box, in the QWERTY grid's order:
// after the three letter rows, then two in the bottom row.
QStringList textExtras(FieldKind field)
{
    switch (field) {
    case FieldKind::Email:
        return { u"-"_s, u"_"_s, u"+"_s, u"@"_s, u".com"_s };
    case FieldKind::Url:
        return { u"-"_s, u"_"_s, u":"_s, u"/"_s, u".com"_s };
    default:
        return { u"-"_s, u"'"_s, u"?"_s, u"!"_s, u"@"_s };
    }
}

// What a number pad has besides the digits.
QString padExtras(FieldKind field)
{
    switch (field) {
    case FieldKind::Number:
        return u".,-+"_s;
    case FieldKind::Phone:
        return u"+-()*#"_s;
    default:
        return { };
    }
}

} // namespace

Layout::Layout(LayoutKind kind, FieldKind field, Page page)
    : m_kind(kind)
    , m_page(page)
{
    using enum KeyAction::Kind;
    if (kind == LayoutKind::Row) {
        makeRow(field, page);
        return;
    }
    if (kind == LayoutKind::Wheel) {
        makeWheel(field, page);
        return;
    }
    const QStringList extra = textExtras(field);

    if (page == Page::Pad) {
        const bool phone = field == FieldKind::Phone;
        QList<Cells> rows { characters(u"123"_s), characters(u"456"_s), characters(u"789"_s),
            phone ? characters(u"*0#"_s)
                  : Cells { Cell(QString()), Cell(u"0"_s), Cell(QString()) } };
        const QString beside = phone ? u"+-()"_s : padExtras(field);
        const Cells actions { Cell(Backspace, 2), Cell(Enter, 2), Cell(Letters, 2),
            Cell(Close, 2) };
        for (int i = 0; i < rows.size(); ++i) {
            if (i < beside.size()) {
                rows[i] << Cell(QString(beside.at(i)));
            }
            rows[i] << actions.at(i);
        }
        makeGrid(rows, gridWidth);
    } else if (page == Page::Symbols) {
        makeGrid(
            {
                characters(u"1234567890"_s) << Cell(Backspace),
                characters(u"!@#$%^&*()-"_s),
                characters(u"_+={}[]|\\/"_s) << Cell(Enter),
                characters(u":;\"'<>,.?~`"_s),
                Cells { Cell(Letters, 2), Cell(Space, 5) } << characters(u"£€"_s) << Cell(Close, 2),
            },
            gridWidth);
    } else if (kind == LayoutKind::Alphabet) {
        makeGrid(
            {
                characters(u"abcdefg123"_s) << Cell(Backspace),
                characters(u"hijklmn456"_s) << Cell(Enter),
                characters(u"opqrstu789"_s) << Cell(Shift),
                characters(u"vwxyz,."_s)
                    << Cell(extra.at(0)) << Cell(u"0"_s) << Cell(extra.at(1)) << Cell(Symbols),
                Cells { Cell(Space, 5), Cell(extra.at(3)), Cell(extra.at(4)), Cell(extra.at(2)),
                    Cell(Close, 3) },
            },
            gridWidth);
    } else {
        makeGrid(
            {
                characters(u"1234567890"_s) << Cell(Backspace),
                characters(u"qwertyuiop"_s) << Cell(extra.at(0)),
                characters(u"asdfghjkl"_s) << Cell(extra.at(1)) << Cell(Enter),
                Cells { Cell(Shift) } << characters(u"zxcvbnm,."_s) << Cell(extra.at(2)),
                Cells { Cell(Symbols, 2), Cell(Space, 5), Cell(extra.at(3)), Cell(extra.at(4)),
                    Cell(Close, 2) },
            },
            gridWidth);
    }
}

Page Layout::firstPage(FieldKind field)
{
    switch (field) {
    case FieldKind::Number:
    case FieldKind::Digits:
    case FieldKind::Phone:
        return Page::Pad;
    default:
        return Page::Letters;
    }
}

int Layout::firstKey() const
{
    const QString text = m_page != Page::Letters ? u"1"_s
        : m_kind == LayoutKind::Qwerty           ? u"q"_s
                                                 : u"a"_s;
    const auto found = std::find_if(
        m_keys.begin(), m_keys.end(), [&text](const Key &key) { return key.text == text; });
    return found == m_keys.end() ? 0 : int(found - m_keys.begin());
}

int Layout::find(KeyAction::Kind action) const
{
    const auto found = std::find_if(
        m_keys.begin(), m_keys.end(), [action](const Key &key) { return key.action == action; });
    return found == m_keys.end() ? -1 : int(found - m_keys.begin());
}

Layout::Cells Layout::characters(const QString &text)
{
    Cells cells;
    for (const QChar character : text) {
        cells << Cell(QString(character));
    }
    return cells;
}

// Everything but the letters and what the keyboard does, for the layouts that
// aren't grids.
Layout::Cells Layout::extras(FieldKind field, Page page)
{
    switch (page) {
    case Page::Pad:
        return characters(u"1234567890"_s + padExtras(field));
    case Page::Symbols:
        return characters(u"1234567890!@#$%^&*()-_+={}[]|\\/:;\"'<>,.?~`£€"_s);
    case Page::Letters:
        break;
    }
    Cells cells = characters(u"abcdefghijklmnopqrstuvwxyz"_s);
    const QStringList extra = textExtras(field);
    for (const QString &text : extra) {
        cells << Cell(text);
    }
    return cells << characters(u",."_s);
}

// Rows of keys. A key with no text that should have some is a gap.
void Layout::makeGrid(const QList<Cells> &rows, qreal visibleWidth)
{
    qreal width = 0;
    for (int row = 0; row < rows.size(); ++row) {
        qreal x = 0;
        for (const Cell &cell : rows.at(row)) {
            if (cell.action != KeyAction::Type || !cell.text.isEmpty()) {
                m_keys << Key { cell.action, cell.text, QRectF(x, row, cell.width, 1) };
            }
            x += cell.width;
        }
        width = std::max(width, x);
    }
    m_size = QSizeF(width, qreal(rows.size()));
    m_visibleWidth = std::max(width, visibleWidth);
}

// Everything in one row that goes round: what the keyboard does is one step left
// of the first letter.
void Layout::makeRow(FieldKind field, Page page)
{
    using enum KeyAction::Kind;
    constexpr qreal wide = 1.5;
    Cells cells { Cell(Close, wide) };
    if (page == Page::Letters) {
        cells << Cell(Symbols, wide) << Cell(Shift, wide);
    } else {
        cells << Cell(Letters, wide);
    }
    cells << Cell(Enter, wide) << Cell(Backspace, wide);
    if (page != Page::Pad) {
        cells << Cell(Space, wide);
    }
    Cells typed = extras(field, page);
    for (Cell &cell : typed) {
        if (cell.text.size() > 1) {
            cell.width = wide;
        }
    }
    makeGrid({ cells << typed }, rowWidth);
    m_visibleWidth = rowWidth;
}

// Petals of up to four keys around a ring: left and right go round it, up and
// down go through a petal. A stand-in for the gamepad's stick and buttons.
void Layout::makeWheel(FieldKind field, Page page)
{
    using enum KeyAction::Kind;
    QList<Cells> petals;
    const Cells typed = extras(field, page);
    for (qsizetype i = 0; i < typed.size(); i += petalSize) {
        petals << typed.mid(i, petalSize);
    }
    // One step left of the first letter, and two steps.
    if (page == Page::Letters) {
        petals << Cells { Cell(Symbols), Cell(Close) };
        petals << Cells { Cell(Space), Cell(Backspace), Cell(Enter), Cell(Shift) };
    } else if (page == Page::Symbols) {
        petals << Cells { Cell(Letters), Cell(Close) };
        petals << Cells { Cell(Space), Cell(Backspace), Cell(Enter) };
    } else {
        petals << Cells { Cell(Backspace), Cell(Enter), Cell(Letters), Cell(Close) };
    }

    // Clockwise: along the top, one at the right end, back along the bottom, and
    // one at the left end.
    const auto count = int(petals.size());
    const int top = (count - 1) / 2;
    const int bottom = count - 2 - top;
    constexpr qreal step = 2 + petalGap;
    m_size = QSizeF(top * step - petalGap + 2 * step, 5);
    const qreal bottomInset = (top - bottom) * step / 2;
    for (int i = 0; i < count; ++i) {
        QPointF corner;
        if (i < top) {
            corner = { step * (i + 1), 0 };
        } else if (i == top) {
            corner = { m_size.width() - 2, 1.5 };
        } else if (i < count - 1) {
            corner = { m_size.width() - step - 2 - bottomInset - step * (i - top - 1), 3 };
        } else {
            corner = { 0, 1.5 };
        }
        m_petals << int(m_keys.size());
        const Cells &petal = petals.at(i);
        for (int slot = 0; slot < petal.size(); ++slot) {
            const int column = slot % 2;
            const int row = slot / 2;
            m_keys << Key { petal.at(slot).action, petal.at(slot).text,
                QRectF(corner + QPointF(column, row), QSizeF(1, 1)) };
        }
    }
    m_visibleWidth = m_size.width();
}

int Layout::move(int from, Direction direction, qreal &column) const
{
    if (from < 0 || from >= m_keys.size()) {
        return m_keys.isEmpty() ? -1 : firstKey();
    }
    if (m_kind == LayoutKind::Wheel) {
        return moveOnWheel(from, direction);
    }
    switch (direction) {
    case Direction::Left:
        return moveSideways(from, -1, column);
    case Direction::Right:
        return moveSideways(from, 1, column);
    case Direction::Up:
        return moveBetweenRows(from, -1, column);
    case Direction::Down:
        return moveBetweenRows(from, 1, column);
    }
    return from;
}

int Layout::moveSideways(int from, int step, qreal &column) const
{
    const auto count = int(m_keys.size());
    int to = from + step;
    if (m_kind == LayoutKind::Row) {
        to = (to + count) % count;
    } else if (to < 0 || to >= count || m_keys.at(to).place.y() != m_keys.at(from).place.y()) {
        // The end of a row in a grid.
        return from;
    }
    column = m_keys.at(to).place.center().x();
    return to;
}

int Layout::moveBetweenRows(int from, int step, qreal column) const
{
    const qreal row = m_keys.at(from).place.y() + step;
    int best = from;
    qreal bestDistance = -1;
    for (int i = 0; i < m_keys.size(); ++i) {
        const QRectF &place = m_keys.at(i).place;
        if (place.y() != row) {
            continue;
        }
        const qreal distance = std::max({ place.left() - column, column - place.right(), 0.0 });
        const bool closer = bestDistance < 0 || distance < bestDistance;
        if (closer) {
            best = i;
            bestDistance = distance;
        }
    }
    return best;
}

int Layout::moveOnWheel(int from, Direction direction) const
{
    const auto count = int(m_petals.size());
    int petal = count - 1;
    while (petal > 0 && m_petals.at(petal) > from) {
        --petal;
    }
    const auto sizeOf = [this, count](int which) {
        const int end = which + 1 < count ? m_petals.at(which + 1) : int(m_keys.size());
        return end - m_petals.at(which);
    };
    const int slot = from - m_petals.at(petal);
    if (direction == Direction::Up || direction == Direction::Down) {
        const int size = sizeOf(petal);
        const int step = direction == Direction::Down ? 1 : size - 1;
        return m_petals.at(petal) + (slot + step) % size;
    }
    const int to = (petal + (direction == Direction::Right ? 1 : count - 1)) % count;
    return m_petals.at(to) + std::min(slot, sizeOf(to) - 1);
}
