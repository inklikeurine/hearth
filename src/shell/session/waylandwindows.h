// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "windows.h"

#include <memory>

class ToplevelManager;

// The session's windows, from labwc through the window-switching protocol
// (wlr-foreign-toplevel-management). Empty outside a Wayland session.
class WaylandWindows : public Windows {
    Q_OBJECT

public:
    explicit WaylandWindows(QObject *parent = nullptr);
    ~WaylandWindows() override;

    [[nodiscard]] QList<WindowInfo> windows() const override;
    void bringToFront(int id) override;
    void minimize(int id) override;
    void makeFullscreen(int id) override;
    void close(int id) override;

private:
    std::unique_ptr<ToplevelManager> m_manager;
};
