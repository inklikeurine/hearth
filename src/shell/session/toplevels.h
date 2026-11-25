// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "qwayland-wlr-foreign-toplevel-management-unstable-v1.h"
#include "windows.h"

#include <QtWaylandClient/QWaylandClientExtensionTemplate>

// One window, as labwc describes it.
class Toplevel : public QObject, public QtWayland::zwlr_foreign_toplevel_handle_v1 {
    Q_OBJECT

public:
    Toplevel(struct ::zwlr_foreign_toplevel_handle_v1 *handle, int id, QObject *parent);
    ~Toplevel() override;

    // Nothing until labwc has sent the whole first description.
    [[nodiscard]] bool isDescribed() const
    {
        return m_described;
    }
    [[nodiscard]] const WindowInfo &info() const
    {
        return m_info;
    }

Q_SIGNALS:
    void changed();
    void closed();

protected:
    // labwc sends what changed piece by piece, then "done".
    void zwlr_foreign_toplevel_handle_v1_title(const QString &title) override;
    void zwlr_foreign_toplevel_handle_v1_app_id(const QString &appId) override;
    void zwlr_foreign_toplevel_handle_v1_state(wl_array *state) override;
    void zwlr_foreign_toplevel_handle_v1_parent(
        struct ::zwlr_foreign_toplevel_handle_v1 *parent) override;
    void zwlr_foreign_toplevel_handle_v1_done() override;
    void zwlr_foreign_toplevel_handle_v1_closed() override;

private:
    WindowInfo m_pending;
    WindowInfo m_info;
    bool m_described = false;
};

class ToplevelManager : public QWaylandClientExtensionTemplate<ToplevelManager>,
                        public QtWayland::zwlr_foreign_toplevel_manager_v1 {
    Q_OBJECT

public:
    ToplevelManager();

    [[nodiscard]] const QList<Toplevel *> &toplevels() const
    {
        return m_toplevels;
    }
    [[nodiscard]] Toplevel *find(int id) const;

Q_SIGNALS:
    void changed();

protected:
    void zwlr_foreign_toplevel_manager_v1_toplevel(
        struct ::zwlr_foreign_toplevel_handle_v1 *handle) override;

private:
    QList<Toplevel *> m_toplevels;
    int m_nextId = 1;
};
