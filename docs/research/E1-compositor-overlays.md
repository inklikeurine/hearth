# E1 — Compositor overlays and app switching (KWin, labwc)
 
Source read on 2026-11-20: KWin master `6145dc6`, labwc master `aabfaf1`. Nothing was run.
Tags: [C] read in primary source, [S] secondary, [M] memory.
 
| Compositor | layer-shell? | Overlay over full-screen app? | Switch apps / return home: how | Source links |
|---|---|---|---|---|
| KWin | Yes [C] | Yes. Layer-shell "overlay" maps to KWin's top-most layer, above the active full-screen window. "Top" maps to a lower layer, so a full-screen app covers it [C] | No wlr-foreign-toplevel, no ext-foreign-toplevel-list (NOT FOUND in source). KWin script: `workspace.activeWindow = w`, `workspace.raiseWindow(w)` [C]. KDE's own `org_kde_plasma_window_management` protocol is offered to non-sandboxed clients [C] | [layer map](https://github.com/KDE/kwin/blob/6145dc6/src/layershellv1window.cpp#L216-L230), [layer order](https://github.com/KDE/kwin/blob/6145dc6/src/effect/globals.h#L148-L162), [fullscreen rule](https://github.com/KDE/kwin/blob/6145dc6/src/window.cpp#L525-L580), [script API](https://github.com/KDE/kwin/blob/6145dc6/src/scripting/workspace_wrapper.h), [protocol filter](https://github.com/KDE/kwin/blob/6145dc6/src/wayland_server.cpp#L132-L165) |
| labwc | Yes [C] | Yes for "overlay" (drawn above all windows). "Top" panels are hidden while a full-screen window is up [C] | wlr-foreign-toplevel-management and ext-foreign-toplevel-list. An "activate" request un-minimizes, switches workspace, focuses and raises the window. A "fullscreen" request is also handled [C] | [stacking table](https://github.com/labwc/labwc/blob/aabfaf1/src/server.c#L615-L640), [top-layer hiding](https://github.com/labwc/labwc/blob/aabfaf1/src/desktop.c#L255-L288), [foreign-toplevel](https://github.com/labwc/labwc/blob/aabfaf1/src/foreign-toplevel/wlr-foreign.c#L44-L52), [focus code](https://github.com/labwc/labwc/blob/aabfaf1/src/desktop.c#L112-L178) |
 
## Q4 — KWin live window view as a QML item
Yes, in name: `WindowThumbnail` (`import org.kde.kwin`), set its `client` (a Window) or `wId` property [C: [header](https://github.com/KDE/kwin/blob/6145dc6/src/scripting/windowthumbnailitem.h#L64-L85)]. KWin's own Overview and Window View effects use it [C: [Overview](https://github.com/KDE/kwin/blob/6145dc6/src/plugins/overview/DesktopView.qml#L33)]. QML effects are built with `SceneEffect` [C: [header](https://github.com/KDE/kwin/blob/6145dc6/src/scripting/scriptedquicksceneeffect.h#L49)]. A 2012 KDE bug says thumbnails repaint on window damage [S: [bug 296066](https://bugs.kde.org/show_bug.cgi?id=296066)].
 
## Not confirmed
- Whether the thumbnail updates continuously on Wayland. Needs a test.
- Any way for a separate program (not a KWin script/effect) to use `WindowThumbnail`: NOT FOUND.
- How a Hearth program would trigger a KWin script (for example over D-Bus): NOT FOUND.
- KWin without plasmashell: the code doesn't restrict layer-shell, and a KDE planning note says third-party clients work standalone [S: [T13496](https://phabricator.kde.org/T13496)]. Never run.
- KWin bug: no configure event when a layer surface is re-shown after unmapping (reported 2025-04-21). Fix status NOT FOUND [S: [bug 460873](https://bugs.kde.org/show_bug.cgi?id=460873)].
- Whether KWin focus-stealing prevention blocks script-driven activation. Needs a test.
## Recommendation (mine, not a decision)
labwc covers questions 1–3 with standard protocols. Only KWin offers question 4.
 
