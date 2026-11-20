# K3 — Existing on-screen keyboards
 
Every cell is [C] (read in the project's own repo) unless marked [S] (secondary source). Read 2026-11-20. Dates are git tag dates. Not ranked.
 
| Keyboard | Licence | Language / toolkit | Last release | Draws itself with | Arrow keys / D-pad? | Link |
|---|---|---|---|---|---|---|
| Maliit | Framework LGPL-2.1; keyboard LGPL-3.0-only (most `src` files BSD) | C++, Qt 5 | Keyboard 2.3.1 (2022-07-21); framework 2.3.0 (2022-07-06) | QML | NOT FOUND (searched `qml`, `src`, `plugins`: no navigation code; untested) | [keyboard](https://github.com/maliit/keyboard), [framework](https://github.com/maliit/framework) |
| Qt Virtual Keyboard | Qt commercial OR GPL-3.0-only | C++ engine, QML front end | v6.11.3 (2026-10-26); newest minor v6.12.0 (2026-09-30) | QML | Yes: `arrowKeyNavigationEnabled` setting; handles Left/Right/Up/Down | [repo](https://github.com/qt/qtvirtualkeyboard) |
| Squeekboard | GPL-3.0 [S] | Rust, GTK3 [S] | v1.43.1 [S]; date NOT FOUND | GTK [S] | NOT FOUND | [repo (unreadable to me)](https://gitlab.gnome.org/World/Phosh/squeekboard) |
| wvkbd | GPL-3.0 (`LICENSE`); two files Wayland-project licence | C | v0.21 (2026-10-27) | Cairo and Pango, into Wayland shared-memory buffers | No: handles only pointer and touch events | [repo](https://github.com/jjsullivan5196/wvkbd) |
| Plasma Bigscreen keyboard = KDE `plasma-keyboard` | Mostly GPL-2.0-only OR GPL-3.0-only; 56 files Qt commercial OR GPL-3.0-only | C++, QML (Qt 6) | v6.8.3 (2026-11-10) | QML | Yes: captures Left/Right/Up/Down (plus Enter/Esc once active). Setting `keyboardNavigationEnabled` defaults to false; Bigscreen sets it true | [plasma-keyboard](https://github.com/KDE/plasma-keyboard), [Bigscreen config](https://github.com/KDE/plasma-bigscreen/blob/master/envmanager/config.h) |
 
## Notes
 
- Bigscreen holds no keyboard code. Its config sets `plasma-keyboard` as the input method and `KWIN_IM_SHOW_ALWAYS=1`.
- plasma-keyboard talks to the compositor over input-method v1. Its README says so.
- Squeekboard's sources, from web search: [GPL-3.0, GTK3](https://linuxlinks.com/squeekboard-wayland-on-screen-keyboard), [v1.43.1](https://t2linux.com/packages/squeekboard).
## Couldn't confirm
 
- Squeekboard: gitlab.gnome.org is blocked for me. Row is [S] only. Whether v1.43.1 is the newest: unknown.
- Maliit "no navigation": absence of search hits, not proof.
- Nothing was run.
 
