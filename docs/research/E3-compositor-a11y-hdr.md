# E3 — Accessibility and HDR (KWin, labwc)
 
Source read on 2026-11-20: KWin master `6145dc6`, labwc master `aabfaf1`, plus git history for "since". Nothing was run.
Tags: [C] read in primary source, [S] secondary, [M] memory.
 
Background: Orca reads app contents through AT-SPI (an accessibility message bus apps use, independent of the compositor). What it needs from the compositor is keyboard access: its modifier key, key echo and key grabs go through the D-Bus interface `org.freedesktop.a11y.KeyboardMonitor` [S: [GNOME forum](https://discourse.gnome.org/t/my-accessibility-stack-and-the-future-on-wayland/35230)].
 
| Compositor | Screen reader | Zoom | Colour filters | HDR | Since version | Source links |
|---|---|---|---|---|---|---|
| KWin | Orca keyboard support: yes, KWin implements `KeyboardMonitor` and `PointerLocator` [C]. Several Orca bugs fixed in 2025–2026, key-repeat one in 2026 [S] | Yes: `zoom` and `magnifier` effects [C] | Yes: `colorblindnesscorrection` effect, plus `invert` [C] | Yes, Wayland only [S]. Design doc: [C] | Orca keyboard: `v6.3.90` (Plasma 6.4 beta). Colour-blindness filter: `v5.27.80` (Plasma 6.0 beta). HDR: `v5.27.80`, shipped in Plasma 6.0 [C/S]. Zoom: long-standing (code from 2006) [C] | [a11y](https://github.com/KDE/kwin/blob/6145dc6/src/a11ymanager.cpp), [filter](https://github.com/KDE/kwin/tree/6145dc6/src/plugins/colorblindnesscorrection), [zoom](https://github.com/KDE/kwin/tree/6145dc6/src/plugins/zoom), [HDR code](https://github.com/KDE/kwin/blob/6145dc6/src/backends/drm/drm_output.cpp#L436), [colour doc](https://github.com/KDE/kwin/blob/6145dc6/doc/color-management.md), [Xaver's blog](https://planet.kde.org/xavers-blog-2023-12-17-an-update-on-hdr-and-color-management-in-kwin) |
| labwc | NOT FOUND: no AT-SPI or `KeyboardMonitor` code in source or docs [C]. labwc is controlled only through Wayland protocols, not D-Bus [S] | Yes: magnifier, actions `ZoomIn`, `ZoomOut`, `ToggleMagnify` [C] | NOT FOUND | Yes: HDR10, off by default (`<core><hdr>`), needs the Vulkan renderer [C]. Also supports colour-management-v1 [C] | Magnifier: `0.7.3`. HDR10: `0.20.0` [C] | [HDR config](https://github.com/labwc/labwc/blob/aabfaf1/docs/labwc-config.5.scd#L245-L249), [NEWS](https://github.com/labwc/labwc/blob/aabfaf1/NEWS.md#L230), [magnifier actions](https://github.com/labwc/labwc/blob/aabfaf1/docs/labwc-actions.5.scd#L445), [Arch wiki](https://wiki.archlinux.org/title/Labwc) |
 
## Not confirmed
- KWin without plasmashell: whether the zoom, filter and shortcut features can be switched on without System Settings and plasmashell. Needs a test.
- KWin: how to enable HDR without System Settings. `kscreen-doctor` can [S: [NVIDIA forum](https://forums.developer.nvidia.com/t/plasma-6-0-hdr-success-report/284564)]. Untested without plasmashell.
- labwc: whether Orca works at all (reads apps, but without key grabs). Needs a test.
- Real HDR on Intel 7th-gen or older-AMD hardware: untested on either compositor.
 
