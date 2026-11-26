// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QPointer>
#include <QQuickItem>
#include <QRect>
#include <QtQml/qqmlregistration.h>

// Asks the compositor to blur the apps behind this item's part of its window,
// through KWindowSystem, which speaks ext-background-effect-v1. A compositor that
// doesn't offer blur (labwc) leaves "available" false, so the panel above it is
// drawn solid instead. One per window: a window has a single blur area.
class CompositorBlur : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool wanted READ wanted WRITE setWanted NOTIFY wantedChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)

public:
    explicit CompositorBlur(QQuickItem *parent = nullptr);
    ~CompositorBlur() override;

    [[nodiscard]] bool wanted() const
    {
        return m_wanted;
    }
    void setWanted(bool wanted);
    // The compositor blurs: what's behind can show through.
    [[nodiscard]] bool available() const
    {
        return m_available;
    }

Q_SIGNALS:
    void wantedChanged();
    void availableChanged();

protected:
    void itemChange(ItemChange change, const ItemChangeData &data) override;

private:
    void follow(QQuickWindow *window);
    void update();

    QPointer<QQuickWindow> m_window;
    QMetaObject::Connection m_frames;
    QRect m_area;
    bool m_wanted = true;
    bool m_available = false;
};
