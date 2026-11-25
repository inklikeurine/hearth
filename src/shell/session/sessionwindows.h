// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "waylandwindows.h"

#include <QtQml/qqmlregistration.h>

// The Wayland library isn't a QML module, so its types are named here.
struct WindowsForeign {
    Q_GADGET
    QML_FOREIGN(Windows)
    QML_ANONYMOUS
};

struct WaylandWindowsForeign {
    Q_GADGET
    QML_FOREIGN(WaylandWindows)
    QML_NAMED_ELEMENT(WaylandWindows)
};
