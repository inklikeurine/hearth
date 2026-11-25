// SPDX-License-Identifier: GPL-3.0-or-later
#include "waylandwindows.h"

#include "toplevels.h"

#include <QGuiApplication>
#include <QtGui/qguiapplication_platform.h>

WaylandWindows::WaylandWindows(QObject *parent)
    : Windows(parent)
{
    if (QGuiApplication::platformName().startsWith(QLatin1String("wayland"))) {
        m_manager = std::make_unique<ToplevelManager>();
        connect(m_manager.get(), &ToplevelManager::changed, this, &Windows::changed);
    }
}

WaylandWindows::~WaylandWindows() = default;

QList<WindowInfo> WaylandWindows::windows() const
{
    QList<WindowInfo> windows;
    if (m_manager) {
        for (const Toplevel *toplevel : m_manager->toplevels()) {
            if (toplevel->isDescribed()) {
                windows << toplevel->info();
            }
        }
    }
    return windows;
}

void WaylandWindows::bringToFront(int id)
{
    const auto *wayland = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    Toplevel *toplevel = m_manager ? m_manager->find(id) : nullptr;
    if (toplevel && wayland && wayland->seat()) {
        // labwc shows a minimized window again when it's activated.
        toplevel->activate(wayland->seat());
    }
}

void WaylandWindows::minimize(int id)
{
    if (Toplevel *toplevel = m_manager ? m_manager->find(id) : nullptr) {
        toplevel->set_minimized();
    }
}

void WaylandWindows::makeFullscreen(int id)
{
    if (Toplevel *toplevel = m_manager ? m_manager->find(id) : nullptr) {
        toplevel->set_fullscreen(nullptr);
    }
}

void WaylandWindows::close(int id)
{
    if (Toplevel *toplevel = m_manager ? m_manager->find(id) : nullptr) {
        toplevel->close();
    }
}
