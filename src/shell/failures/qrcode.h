// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QQuickPaintedItem>

// A QR code: black on white with a white edge, whatever the theme, because phones
// read nothing else reliably. Its size in modules is 0 when TEXT is too long.
class QrCode : public QQuickPaintedItem {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(int modules READ modules NOTIFY textChanged)

public:
    using QQuickPaintedItem::QQuickPaintedItem;

    [[nodiscard]] QString text() const
    {
        return m_text;
    }
    void setText(const QString &text);
    [[nodiscard]] int modules() const
    {
        return m_modules;
    }
    void paint(QPainter *painter) override;

Q_SIGNALS:
    void textChanged();

private:
    QString m_text;
    int m_modules = 0;
    QList<bool> m_dark;
};
