// SPDX-License-Identifier: GPL-3.0-or-later
#include "keyboardmodel.h"

using namespace Qt::StringLiterals;

int KeyboardModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : int(m_layout.keys().size());
}

QVariant KeyboardModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_layout.keys().size()) {
        return { };
    }
    const Key &key = m_layout.keys().at(index.row());
    switch (role) {
    case Action:
        return key.action;
    case Text:
        return textOf(key);
    case Place:
        return key.place;
    default:
        return { };
    }
}

QHash<int, QByteArray> KeyboardModel::roleNames() const
{
    return { { Action, "does" }, { Text, "typed" }, { Place, "place" } };
}

qreal KeyboardModel::currentX() const
{
    return m_current < 0 ? 0 : m_layout.keys().at(m_current).place.center().x();
}

void KeyboardModel::open(LayoutKind layout, FieldKind field)
{
    m_kind = layout;
    m_field = field;
    setShift(Off);
    show(Layout::firstPage(field));
}

void KeyboardModel::show(Page page)
{
    beginResetModel();
    m_layout = Layout(m_kind, m_field, page);
    m_current = -1;
    endResetModel();
    Q_EMIT sizeChanged();
    setCurrent(m_layout.firstKey());
}

void KeyboardModel::move(Direction direction)
{
    const int to = m_layout.move(m_current, direction, m_column);
    if (to != m_current) {
        // The column stays where move() left it.
        m_current = to;
        Q_EMIT currentIndexChanged();
    }
}

void KeyboardModel::setCurrent(int index)
{
    m_current = index;
    m_column = currentX();
    Q_EMIT currentIndexChanged();
}

void KeyboardModel::pressKey(int index)
{
    if (index >= 0 && index < m_layout.keys().size()) {
        setCurrent(index);
        press();
    }
}

void KeyboardModel::press()
{
    if (m_current < 0) {
        return;
    }
    Q_EMIT pressed();
    const Key key = m_layout.keys().at(m_current);
    switch (key.action) {
    case KeyAction::Type:
        Q_EMIT typed(textOf(key));
        if (m_shift == Once) {
            setShift(Off);
        }
        break;
    case KeyAction::Space:
        Q_EMIT typed(u" "_s);
        break;
    case KeyAction::Backspace:
        Q_EMIT backspace();
        break;
    case KeyAction::Enter:
        Q_EMIT enter();
        break;
    case KeyAction::Shift:
        setShift(m_shift == Off ? Once : m_shift == Once ? Locked : Off);
        break;
    case KeyAction::Symbols:
        show(Page::Symbols);
        // A second press goes back.
        setCurrent(m_layout.find(KeyAction::Letters));
        break;
    case KeyAction::Letters:
        show(Page::Letters);
        setCurrent(m_layout.find(KeyAction::Symbols));
        break;
    case KeyAction::Close:
        Q_EMIT close();
        break;
    }
}

void KeyboardModel::setShift(ShiftState shift)
{
    if (shift == m_shift) {
        return;
    }
    m_shift = shift;
    if (rowCount() > 0) {
        Q_EMIT dataChanged(index(0), index(rowCount() - 1), { Text });
    }
    Q_EMIT shiftChanged();
}

QString KeyboardModel::textOf(const Key &key) const
{
    // Only letters: ".com" stays as it is.
    return m_shift != Off && key.text.size() == 1 ? key.text.toUpper() : key.text;
}
