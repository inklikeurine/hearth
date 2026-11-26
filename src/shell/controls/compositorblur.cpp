// SPDX-License-Identifier: GPL-3.0-or-later
#include "compositorblur.h"

#include <KWindowEffects>
#include <QQuickWindow>
#include <QRegion>

CompositorBlur::CompositorBlur(QQuickItem *parent)
    : QQuickItem(parent)
{
}

CompositorBlur::~CompositorBlur()
{
    follow(nullptr);
}

void CompositorBlur::setWanted(bool wanted)
{
    if (wanted != m_wanted) {
        m_wanted = wanted;
        Q_EMIT wantedChanged();
        update();
    }
}

void CompositorBlur::itemChange(ItemChange change, const ItemChangeData &data)
{
    if (change == ItemSceneChange) {
        follow(data.window);
    } else if (change == ItemVisibleHasChanged) {
        update();
    }
    QQuickItem::itemChange(change, data);
}

void CompositorBlur::follow(QQuickWindow *window)
{
    if (m_window == window) {
        return;
    }
    disconnect(m_frames);
    if (m_window && !m_area.isEmpty()) {
        KWindowEffects::enableBlurBehind(m_window, false);
    }
    m_area = { };
    m_window = window;
    if (window) {
        // The panel moves as it slides in, and nothing says when an item's place
        // in the window changes; each frame does. Nothing is sent unless it moved.
        m_frames = connect(window, &QQuickWindow::afterAnimating, this, &CompositorBlur::update);
    }
    update();
}

void CompositorBlur::update()
{
    // KWindowSystem has no signal for blur coming and going, so look each time.
    const bool available = KWindowEffects::isEffectAvailable(KWindowEffects::BlurBehind);
    if (available != m_available) {
        m_available = available;
        Q_EMIT availableChanged();
    }
    if (!m_window) {
        return;
    }
    const QRect area = m_wanted && m_available && isVisible()
        ? mapRectToScene(boundingRect()).toAlignedRect()
        : QRect();
    if (area == m_area) {
        return;
    }
    m_area = area;
    // An empty area would mean the whole window to KWindowSystem.
    KWindowEffects::enableBlurBehind(m_window, !area.isEmpty(), QRegion(area));
}
