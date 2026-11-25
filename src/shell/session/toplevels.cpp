// SPDX-License-Identifier: GPL-3.0-or-later
#include "toplevels.h"

#include <algorithm>
#include <span>

namespace {

// The version Fedora 45's labwc offers; it's the first to say which window is
// another's dialog.
constexpr int protocolVersion = 3;

} // namespace

Toplevel::Toplevel(struct ::zwlr_foreign_toplevel_handle_v1 *handle, int id, QObject *parent)
    : QObject(parent)
    , QtWayland::zwlr_foreign_toplevel_handle_v1(handle)
{
    m_pending.id = id;
}

Toplevel::~Toplevel()
{
    destroy();
}

void Toplevel::zwlr_foreign_toplevel_handle_v1_title(const QString &title)
{
    m_pending.title = title;
}

void Toplevel::zwlr_foreign_toplevel_handle_v1_app_id(const QString &appId)
{
    m_pending.appId = appId;
}

void Toplevel::zwlr_foreign_toplevel_handle_v1_state(wl_array *state)
{
    const auto *values = static_cast<const uint32_t *>(state->data);
    const std::span states(values, state->size / sizeof(uint32_t));
    const auto has = [states](uint32_t value) {
        return std::find(states.begin(), states.end(), value) != states.end();
    };
    m_pending.active = has(state_activated);
    m_pending.minimized = has(state_minimized);
    m_pending.fullscreen = has(state_fullscreen);
}

void Toplevel::zwlr_foreign_toplevel_handle_v1_parent(
    struct ::zwlr_foreign_toplevel_handle_v1 *parent)
{
    m_pending.hasParent = parent != nullptr;
}

void Toplevel::zwlr_foreign_toplevel_handle_v1_done()
{
    m_info = m_pending;
    m_described = true;
    Q_EMIT changed();
}

void Toplevel::zwlr_foreign_toplevel_handle_v1_closed()
{
    Q_EMIT closed();
}

ToplevelManager::ToplevelManager()
    : QWaylandClientExtensionTemplate<ToplevelManager>(protocolVersion)
{
    initialize();
}

Toplevel *ToplevelManager::find(int id) const
{
    const auto found = std::find_if(m_toplevels.begin(), m_toplevels.end(),
        [id](const Toplevel *toplevel) { return toplevel->info().id == id; });
    return found == m_toplevels.end() ? nullptr : *found;
}

void ToplevelManager::zwlr_foreign_toplevel_manager_v1_toplevel(
    struct ::zwlr_foreign_toplevel_handle_v1 *handle)
{
    auto *toplevel = new Toplevel(handle, m_nextId++, this);
    m_toplevels << toplevel;
    connect(toplevel, &Toplevel::changed, this, &ToplevelManager::changed);
    connect(toplevel, &Toplevel::closed, this, [this, toplevel] {
        m_toplevels.removeOne(toplevel);
        toplevel->deleteLater();
        Q_EMIT changed();
    });
}
