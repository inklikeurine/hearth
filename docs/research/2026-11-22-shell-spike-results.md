# Shell spike: results so far

*Claude, 2026-11-22. The spike code is on the local-only `spikes` branch (`spikes/shell/`, its README has the details). Everything below ran headless: labwc with no screen in the dev box, keys pressed with wtype, windows checked with wlrctl, pictures saved by the shell. The latest run passed 43 of 43 checks, twice in a row. Nobody has watched it on a screen yet.*

## Spike checks (design doc, section 5.4)

| Check | Result |
|---|---|
| 1. Bare home screen | Works: a full-screen window with the user's background, the status bar, tiles in rows of five with the next row peeking in, arrow keys moving focus and scrolling rows |
| 2. Firefox full screen | Works: kiosk mode, full screen, on top of the home window |
| 3. Menu over Firefox | Works: a layer-shell overlay over the full-screen app, taking the keys while open |
| 5. Going home | Works: Home (a labwc key binding) minimizes the app and brings the home window forward; the app's tile brings it back un-minimized, without restarting it |
| 6. Back key | Works. labwc takes the remote's Back; the shell sends a `back` action over native messaging to a TV-navigation extension, which closes the page's open menu, else goes back a page, else leaves the app (the home screen comes back). Firefox stayed full screen; the pages never saw Back or Escape. Slowest round trip, shell → extension → page → shell: 7–8 ms (target: 50 ms) |
| 12. Forcing full screen | Works with a labwc window rule using `ToggleFullscreen`: foot, which opens as a window, goes full screen, and Firefox in kiosk mode stays full screen. `Maximize` didn't take effect at all |

Checks 4 (typing), 7 (phone typing) and 8–11 (real screen, RAM, overlay over video, TV colour range) are still to do.

## Found on the way

- **The home screen as a desktop** needs the window-switching protocol (wlr-foreign-toplevel-management), from Fedora's `wlr-protocols-devel`. With it the shell knows which app is in front, minimizes it, brings it back and closes it.
- **Fedora 45 packaging gap:** `qt6-qtwayland-devel` lacks a file (`qt6waylandclient_metatypes.json`) that Qt's build tools ask for when a QML program links the Wayland client library directly. Linking it through a small library of its own avoids it.
- **Firefox requires Mozilla's signature** for extensions it installs for good (Fedora's build: `MOZ_REQUIRE_SIGNING: true`, release channel). The spike installs the extension as a temporary add-on through Firefox's debugging channel, opened on a socket file in the user's private runtime folder. That's for testing only: anything running as the user can drive that Firefox while the channel is open.
- **Firefox keeps extensions out of `file://` pages**, so local test pages have to be served over HTTP. Real sites (`https://`) are fine.
- **labwc** runs its `-S` session command without a shell, so `VAR=value command` fails; `env VAR=value command` works.
- **The dev box** leaks the host desktop's settings into apps (`DISPLAY`, `SESSION_MANAGER`, `GTK_MODULES`, `XDG_DATA_DIRS`); the spike strips them.

## What it means for the plan (Claude's reading, not decided)

- The Back design in section 6.1 works as planned, and fast. What the product needs on top: the extension signed by Mozilla (unlisted, as Leanback for Dropout is), and the native messaging host's manifest installed system-wide in the image rather than in each user's home.
- The spike catches only the remote's Back key, through labwc. The real input service grabs remote devices so keyboards keep typing (section 6.2); a keyboard's Esc still reaches Firefox in the spike.
- Back in native apps needs the app adapters from section 6.1; the spike answers "no adapter".

## Not confirmed

- Anything on a real screen or real hardware, with the real air mouse.
- Real sites: what counts as a site's "menu" needs rules per site, as Leanback has for Dropout.
