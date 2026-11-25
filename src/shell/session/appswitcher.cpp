// SPDX-License-Identifier: GPL-3.0-or-later
#include "appswitcher.h"

#include <algorithm>
#include <utility>

using namespace Qt::StringLiterals;

namespace {

constexpr int mostFullscreens = 3;

bool isHome(const WindowInfo &window)
{
    return window.appId == u"hearth-shell"_s;
}

bool sameName(const QString &a, const QString &b)
{
    return a.compare(b, Qt::CaseInsensitive) == 0;
}

} // namespace

AppSwitcher::AppSwitcher(QObject *parent)
    : QObject(parent)
{
    m_closeTimer.setSingleShot(true);
    connect(&m_closeTimer, &QTimer::timeout, this, &AppSwitcher::closeTimedOut);
}

void AppSwitcher::setWindows(Windows *windows)
{
    if (m_windows) {
        m_windows->disconnect(this);
    }
    m_windows = windows;
    if (m_windows) {
        connect(m_windows, &Windows::changed, this, &AppSwitcher::update);
    }
    Q_EMIT setUp();
    update();
}

void AppSwitcher::setLauncher(AppLauncher *launcher)
{
    if (m_launcher) {
        m_launcher->disconnect(this);
    }
    m_launcher = launcher;
    if (m_launcher) {
        connect(m_launcher, &AppLauncher::finished, this, &AppSwitcher::appEnded);
        connect(m_launcher, &AppLauncher::failed, this, &AppSwitcher::launchFailed);
    }
    Q_EMIT setUp();
}

QList<WindowInfo> AppSwitcher::appWindows() const
{
    QList<WindowInfo> windows = m_windows ? m_windows->windows() : QList<WindowInfo>();
    windows.removeIf(isHome);
    return windows;
}

const DesktopEntry *AppSwitcher::appFor(const WindowInfo &window) const
{
    if (!m_apps) {
        return nullptr;
    }
    const auto &apps = m_apps->apps();
    const auto found = std::find_if(apps.begin(), apps.end(), [&window](const DesktopEntry &app) {
        return sameName(app.id, window.appId)
            || (!app.windowClass.isEmpty() && sameName(app.windowClass, window.appId));
    });
    return found == apps.end() ? nullptr : &*found;
}

QString AppSwitcher::nameOf(const QString &id) const
{
    if (m_launched && m_launched->id == id) {
        return m_launched->name;
    }
    return id;
}

void AppSwitcher::update()
{
    if (!m_windows) {
        return;
    }
    const QList<WindowInfo> windows = m_windows->windows();
    makeFullscreen(windows);
    updateFront(windows);
    updateRunning(windows);

    const bool noWindows = appWindows().isEmpty();
    const bool noPrograms = !m_launcher || m_launcher->running().isEmpty();
    if (noWindows && noPrograms) {
        m_closing = false;
        m_closeTimer.stop();
        if (m_next) {
            const DesktopEntry next = *m_next;
            m_next.reset();
            start(next);
        }
    } else if (noWindows && m_hadWindow && !m_closing) {
        // Its windows are gone but its program isn't: wait a little, then end it.
        m_closing = true;
        m_closeTimer.start(m_closeWait);
    }
    if (noWindows && m_openingName.isEmpty() && !m_homeInFront) {
        goHome();
    }
}

void AppSwitcher::makeFullscreen(const QList<WindowInfo> &windows)
{
    QHash<int, int> counts;
    for (const WindowInfo &window : windows) {
        int count = m_fullscreened.value(window.id);
        // An app that keeps leaving full screen is let be.
        if (!window.hasParent && !window.fullscreen && count < mostFullscreens) {
            ++count;
            m_windows->makeFullscreen(window.id);
        }
        counts.insert(window.id, count);
    }
    m_fullscreened = std::move(counts);
}

void AppSwitcher::updateFront(const QList<WindowInfo> &windows)
{
    const auto active = std::find_if(
        windows.begin(), windows.end(), [](const WindowInfo &window) { return window.active; });
    // While an overlay has the keyboard no window is active: nothing has changed.
    if (active == windows.end() || isHome(*active) == m_homeInFront) {
        return;
    }
    m_homeInFront = !m_homeInFront;
    Q_EMIT changed();
    if (m_homeInFront) {
        Q_EMIT homeShown();
    }
}

void AppSwitcher::updateRunning(const QList<WindowInfo> &windows)
{
    const WindowInfo *front = nullptr;
    for (const WindowInfo &window : windows) {
        if (!isHome(window) && (!front || window.active)) {
            front = &window;
        }
    }
    if (!front) {
        if (m_next) {
            setRunning(m_next->id, m_next->name, m_next->name);
        } else if (m_launched && m_launcher && m_launcher->running().contains(m_launched->id)) {
            const QString opening = m_hadWindow ? QString() : m_launched->name;
            setRunning(m_launched->id, m_launched->name, opening);
        } else {
            setRunning({ }, { }, { });
        }
        return;
    }
    if (!m_hadWindow) {
        m_hadWindow = true;
    }
    const QString opening = m_next ? m_next->name : QString();
    if (const DesktopEntry *app = appFor(*front)) {
        setRunning(app->id, app->name, opening);
    } else if (m_launched && m_launcher && m_launcher->running().contains(m_launched->id)) {
        // A window whose name matches no app: it's the one that was started.
        setRunning(m_launched->id, m_launched->name, opening);
    } else {
        setRunning(front->appId, front->title.isEmpty() ? front->appId : front->title, opening);
    }
}

void AppSwitcher::setRunning(const QString &id, const QString &name, const QString &opening)
{
    if (id != m_runningId || name != m_runningName || opening != m_openingName) {
        m_runningId = id;
        m_runningName = name;
        m_openingName = opening;
        Q_EMIT changed();
    }
}

void AppSwitcher::open(int index)
{
    if (!m_apps || !m_launcher || index < 0 || index >= m_apps->count()) {
        return;
    }
    const DesktopEntry app = m_apps->apps().at(index);
    m_next.reset();
    if (!appWindows().isEmpty() && app.id == m_runningId) {
        returnToApp();
        return;
    }
    if (m_launcher->running().contains(app.id) && !m_closing) {
        return; // Still starting.
    }
    if (!appWindows().isEmpty() || !m_launcher->running().isEmpty()) {
        m_next = app;
        askToClose();
        update();
        return;
    }
    start(app);
}

void AppSwitcher::openById(const QString &appId)
{
    if (!m_apps) {
        return;
    }
    const auto &apps = m_apps->apps();
    const auto found = std::find_if(
        apps.begin(), apps.end(), [&appId](const DesktopEntry &app) { return app.id == appId; });
    if (found != apps.end()) {
        open(int(found - apps.begin()));
    }
}

void AppSwitcher::start(const DesktopEntry &app)
{
    m_launched = app;
    m_hadWindow = false;
    m_closing = false;
    m_terminated = false;
    m_launcher->start(app);
    update();
}

void AppSwitcher::goHome()
{
    m_next.reset();
    if (!m_windows) {
        return;
    }
    for (const WindowInfo &window : m_windows->windows()) {
        if (isHome(window)) {
            m_windows->bringToFront(window.id);
        } else if (!window.minimized && !window.hasParent) {
            m_windows->minimize(window.id);
        }
    }
    update();
}

void AppSwitcher::returnToApp()
{
    m_next.reset();
    // Its dialogs last, so they end up on top.
    QList<WindowInfo> windows = appWindows();
    std::stable_sort(windows.begin(), windows.end(),
        [](const auto &a, const auto &b) { return a.hasParent < b.hasParent; });
    for (const WindowInfo &window : std::as_const(windows)) {
        m_windows->bringToFront(window.id);
    }
    update();
}

void AppSwitcher::closeApp()
{
    m_next.reset();
    askToClose();
    update();
}

void AppSwitcher::askToClose()
{
    const QList<WindowInfo> windows = appWindows();
    for (const WindowInfo &window : windows) {
        // Closing a dialog answers it, which isn't ours to do.
        if (!window.hasParent) {
            m_windows->close(window.id);
        }
    }
    m_closing = true;
    if (windows.isEmpty() && m_launcher) {
        m_terminated = true;
        m_launcher->terminateAll();
    }
    m_closeTimer.start(m_closeWait);
}

void AppSwitcher::closeTimedOut()
{
    if (!appWindows().isEmpty()) {
        m_closing = false;
        // What it's asking about is on its own screen, so it can't be left hidden.
        Q_EMIT stillOpen(m_runningName);
        return;
    }
    if (!m_launcher || m_launcher->running().isEmpty()) {
        return;
    }
    if (m_terminated) {
        m_launcher->killAll();
    } else {
        m_terminated = true;
        m_launcher->terminateAll();
        m_closeTimer.start(m_closeWait);
    }
}

void AppSwitcher::forceClose()
{
    m_closing = true;
    m_terminated = true;
    if (m_launcher) {
        m_launcher->killAll();
    }
    // A window whose program we didn't start can only be asked.
    for (const WindowInfo &window : appWindows()) {
        m_windows->close(window.id);
    }
}

void AppSwitcher::appEnded(const QString &id, bool withError, bool crashed)
{
    const bool ours = m_launched && m_launched->id == id;
    const QString name = nameOf(id);
    const bool unexpected = ours && !m_closing && !m_terminated;
    const bool hadWindow = m_hadWindow;
    if (ours && !m_next) {
        m_launched.reset();
    }
    update();
    if (unexpected && withError && !hadWindow) {
        Q_EMIT endedEarly(id, name);
    } else if (unexpected && crashed) {
        Q_EMIT this->crashed(id, name);
    }
}

void AppSwitcher::launchFailed(const QString &id)
{
    const QString name = nameOf(id);
    m_launched.reset();
    update();
    Q_EMIT couldNotStart(id, name);
}
