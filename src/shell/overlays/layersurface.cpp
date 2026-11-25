// SPDX-License-Identifier: GPL-3.0-or-later
#include "layersurface.h"

#include <LayerShellQt/Window>
#include <QGuiApplication>

void LayerSurface::setUp(QQuickWindow *window, const QString &name, bool takesKeyboard)
{
    using LayerShellQt::Window;
    if (!window || !QGuiApplication::platformName().startsWith(QLatin1String("wayland"))) {
        return;
    }
    if (!takesKeyboard) {
        // The pointer goes through it, to what's behind.
        window->setFlag(Qt::WindowTransparentForInput);
    }
    Window *layer = Window::get(window);
    layer->setLayer(Window::LayerOverlay);
    layer->setAnchors(
        { Window::AnchorTop, Window::AnchorBottom, Window::AnchorLeft, Window::AnchorRight });
    // The whole screen, whatever else has kept an edge for itself.
    layer->setExclusiveZone(-1);
    layer->setKeyboardInteractivity(
        takesKeyboard ? Window::KeyboardInteractivityExclusive : Window::KeyboardInteractivityNone);
    layer->setScope(u"hearth-" + name);
}
