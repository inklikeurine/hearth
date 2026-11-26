// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "layout.h"

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

// The keys on screen: which is highlighted, Shift, and what pressing one does.
class KeyboardModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("the program makes it")
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)
    // The highlighted key's middle, in key widths.
    Q_PROPERTY(qreal currentX READ currentX NOTIFY currentIndexChanged)
    Q_PROPERTY(QSizeF size READ size NOTIFY sizeChanged)
    Q_PROPERTY(qreal visibleWidth READ visibleWidth NOTIFY sizeChanged)
    Q_PROPERTY(ShiftState shift READ shift NOTIFY shiftChanged)

public:
    enum Role { Action = Qt::UserRole, Text, Place };
    // Once: for the next letter only. Locked: until pressed again.
    enum ShiftState { Off, Once, Locked };
    Q_ENUM(ShiftState)

    using QAbstractListModel::QAbstractListModel;

    [[nodiscard]] int rowCount(const QModelIndex &parent = { }) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] int currentIndex() const
    {
        return m_current;
    }
    [[nodiscard]] qreal currentX() const;
    [[nodiscard]] QSizeF size() const
    {
        return m_layout.size();
    }
    [[nodiscard]] qreal visibleWidth() const
    {
        return m_layout.visibleWidth();
    }
    [[nodiscard]] ShiftState shift() const
    {
        return m_shift;
    }
    [[nodiscard]] const Layout &keys() const
    {
        return m_layout;
    }

    // Starts afresh for a text box.
    void open(LayoutKind layout, FieldKind field);
    void move(Direction direction);
    // Presses the highlighted key.
    void press();
    // A key pressed with the pointer: it takes the highlight too.
    Q_INVOKABLE void pressKey(int index);

Q_SIGNALS:
    // Any key, with the remote or the pointer.
    void pressed();
    void typed(const QString &text);
    void backspace();
    void enter();
    void close();

    void currentIndexChanged();
    // Not layoutChanged: QAbstractItemModel already has a signal by that name.
    void sizeChanged();
    void shiftChanged();

private:
    void show(Page page);
    void setCurrent(int index);
    void setShift(ShiftState shift);
    // What a key types now.
    [[nodiscard]] QString textOf(const Key &key) const;

    LayoutKind m_kind = LayoutKind::Qwerty;
    FieldKind m_field = FieldKind::Text;
    Layout m_layout;
    int m_current = -1;
    qreal m_column = 0;
    ShiftState m_shift = Off;
};
