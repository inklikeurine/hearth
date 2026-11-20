# E2 — Compositor input protocols (KWin, labwc)
 
Source read on 2026-11-20: KWin master `6145dc6`, labwc master `aabfaf1`, plus git history for "since". Nothing was run.
Tags: [C] read in primary source, [S] secondary, [M] memory.
 
| Compositor | input-method | text-input | virtual-keyboard | libei | RemoteDesktop portal (backend) | Since version | Source links |
|---|---|---|---|---|---|---|---|
| KWin | v1 only [C]. v2 NOT FOUND. Offered only to the one program KWin starts as the input method, not to ordinary apps [C] | v2 and v3 [C]. v1: no manager registered; only its constants are used [C] | NOT FOUND. KWin's "VirtualKeyboard" is its own D-Bus on-screen-keyboard toggle. Its alternative is the KDE-specific `org_kde_kwin_fake_input`, restricted to non-sandboxed clients [C] | Yes, as a plugin (`eis`), built only if libeis is found at build time [C] | Backend: xdg-desktop-portal-kde. It calls KWin's `org.kde.KWin.EIS.RemoteDesktop` D-Bus interface [C] | EIS: first tag `v6.0.90` (Plasma 6.1 beta) [C]. text-input v3 code: first KWin tag `v5.24.90` [C]. Others not checked individually | [input-method](https://github.com/KDE/kwin/blob/6145dc6/src/wayland_server.cpp#L142), [text-input](https://github.com/KDE/kwin/blob/6145dc6/src/inputmethod.cpp#L103-L104), [fake_input](https://github.com/KDE/kwin/blob/6145dc6/src/wayland_server.cpp#L132-L140), [EIS](https://github.com/KDE/kwin/blob/6145dc6/src/plugins/eis/eisbackend.cpp#L102), [libeis option](https://github.com/KDE/kwin/blob/6145dc6/CMakeLists.txt#L413), [portal](https://raw.githubusercontent.com/KDE/xdg-desktop-portal-kde/master/src/remotedesktop.cpp) |
| labwc | v2 only [C] (via wlroots) | v3 only [C] | v1 [C] (also virtual-pointer) | NOT FOUND | NOT FOUND. xdg-desktop-portal-wlr lists only Screenshot and ScreenCast [S] | input-method + text-input v3: `0.7.2`. virtual-keyboard: `0.6.0` [C] | [IME](https://github.com/labwc/labwc/blob/aabfaf1/src/server.c#L734-L737), [virtual-keyboard](https://github.com/labwc/labwc/blob/aabfaf1/src/seat.c#L708-L713), [NEWS](https://github.com/labwc/labwc/blob/aabfaf1/NEWS.md#L1672), [wlr portal](https://github.com/emersion/xdg-desktop-portal-wlr) |
 
## Not confirmed
- KWin: whether Fedora's KWin package is built with libeis (build option is on only if found).
- KWin: whether xdg-desktop-portal-kde picks itself as the portal backend without plasmashell (needs a portal config test). Its code doesn't mention plasmashell [C].
- KWin: "since" for text-input v2 and input-method v1 individually.
- labwc: any RemoteDesktop backend that works with it. I found none, not proof that none exists.
 
