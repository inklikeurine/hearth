# E4 — Compositor footprint (KWin, labwc)
 
Checked 2026-11-20. Tags: [C] read in primary source, [S] secondary, [M] memory.
 
## 1. Idle RAM (published measurements)
- **labwc:** 76 MB resident (RSS) for the `labwc` process. Measured by Luigi (grigio.org blog), published 28 March 2026. System, distro, total RAM and whether the session was idle are NOT stated. The post contradicts itself: a later table gives 105 MB. Weak evidence [S: [post](https://grigio.org/is-systemd-bloat-real-dinit-vs-systemd-which-session-uses-less-memory-with-labwc/)].
- **KWin without plasmashell:** NOT FOUND. Only full-Plasma-session reports exist: a KDE bug report from Feb 2022 says KWin starts a Wayland session at about 50 MB and then grows [S: [bug 450181](https://bugs.kde.org/show_bug.cgi?id=450181)]; a Fedora user's report (2025) shows 226 MB RSS before launching a game [S: [bug 497056](https://bugs.kde.org/show_bug.cgi?id=497056)]. Neither is idle-without-shell, and both come with leak reports.
## 2. Runtime libraries (Fedora 45 package pages)
 
| Compositor | Fedora package | Main runtime dependencies | Source link |
|---|---|---|---|
| labwc | `labwc` 0.20.2-2.fc45 [C] | wlroots, libwayland (client, server, cursor), libinput, libxkbcommon, cairo, pango, glib2, libxml2, librsvg2, libpng, pixman, libsfdo, libxcb, xcb-util-wm, mesa-dri-drivers, xdg-desktop-portal-wlr. No Qt or GTK [C] | [Fedora](https://packages.fedoraproject.org/pkgs/labwc/labwc/fedora-45.html), [meson.build](https://github.com/labwc/labwc/blob/aabfaf1/meson.build#L57-L102) |
| KWin | `kwin`, `kwin-common`, `kwin-libs`, all 6.8.1-2.fc45 [C]. `kwin-wayland` is not listed for F45 | **Qt 6:** base, declarative, multimedia, svg, tools. **KF6 (KDE Frameworks):** kconfig, kcoreaddons, kcrash, kdbusaddons, kdeclarative, kglobalaccel, ki18n, kidletime, kirigami, knotifications, krunner, ksvg, kwindowsystem, kcmutils, kcolorscheme, kpackage, kservice, kxmlgui, kauth, kguiaddons, knewstuff, kwidgetsaddons. **Plasma-side:** libplasma, kdecoration, kscreenlocker, kglobalacceld, plasma-activities, kwayland, aurorae, knighttime. **Other:** libeis, pipewire, libinput, libdrm, gbm, libdisplay-info, lcms2, vulkan-loader, Xwayland, libcanberra, qaccessibilityclient | [kwin](https://packages.fedoraproject.org/pkgs/kwin/kwin/fedora-45.html), [kwin-common](https://packages.fedoraproject.org/pkgs/kwin/kwin-common/fedora-45.html), [kwin-libs](https://packages.fedoraproject.org/pkgs/kwin/kwin-libs/fedora-45.html), [CMakeLists](https://github.com/KDE/kwin/blob/6145dc6/CMakeLists.txt#L93-L130) |
 
KWin's own build file marks `libplasma` as "required at runtime for several QML effects", not for starting. Fedora requires it anyway. `kdecoration` is a hard requirement; `kglobalacceld` and `kscreenlocker` are hard requirements when the global-shortcuts and screen-locker features are built, which is the default [C].
 
## Not confirmed
- Dependency lists are for 6.8.1-2.fc45 (testing). Stable is 6.8.3 and may differ.
- Installed size and idle RAM of KWin plus these libraries without plasmashell: needs measuring on the hardware floor.
 
