# Decision 0016: Theming: Hearth's own tokens with no Kirigami, preview-first Appearance, day/night toggle, no fonts in theme packs, Atkinson Hyperlegible Next by default

Accepted 2026-11-22 by the lead. Replaces decision 0009 (theming presets and packs, accepted 2026-11-19), which planned a small bridge for leftover Kirigami controls. Related: design doc, sections 2 and 9; [theming research](../research/2026-11-19-theming.md); [open-source fonts](../research/B5-oss-fonts.md); [Borrow from Bigscreen, write the shell fresh](0012-borrow-from-bigscreen-write-shell-fresh.md).

## Why it changed

Decision 0009 was written when the shell was going to be a fork of KDE Plasma Bigscreen. Bigscreen's shared controls and home screen use Kirigami's theme and size names (Kirigami is KDE's QML toolkit), so 0009 chose "route C": Hearth's theme drives the screens directly, plus a small QML file passing Hearth's colours to any Kirigami controls that remain.

Since then:
- The shell is written fresh, borrowing parts from Bigscreen ([decision 0012](0012-borrow-from-bigscreen-write-shell-fresh.md)), so there's no fork to move off Kirigami.
- The shell spike built the home screen, menu and on-screen keyboard with Hearth's own controls and theme tokens only, with no gap Kirigami would have filled.
- The lead, 2026-11-22: no Kirigami ("Kirigami: no?", confirmed as "No Kirigami").
- The lead, 2026-11-22, on fonts: Atkinson Hyperlegible Next as the default, as Claude recommended, "with a settings menu to change them once we design the settings menu/app".

## Decision

1. **No Kirigami.** Hearth's own controls and theme tokens drive every screen. No bridge file.
2. **Fonts:** no custom fonts in theme packs. Hearth ships a set of open-source fonts in the OS image, and theme authors pick a family from those. The default is Atkinson Hyperlegible Next, with Noto Sans for characters it lacks. A settings page lets the user change fonts; it's designed with the settings app.
3. Appearance settings are preview first, then apply.
4. Automatic day/night switching is user-settable: a toggle, off by default. On shows a Day theme and a Night theme dropdown, pre-filled from the theme's own light and dark modes unless the user has set them, switching at two fixed times the user sets (no sunrise/sunset, which would need the box's location). Off: one theme all the time; a theme with two modes shows its author's default mode. No playback check.
5. The preset set and default look are scoped from scratch (research series A and B in the lead's log).
6. Theme packs are data only, never code (as in the design doc).

Points 3 to 6 are unchanged from decision 0009.

## Consequences

- One fewer dependency (KDE Frameworks' Kirigami) and no bridge to keep in step with it.
- Every control Hearth needs is Hearth's own to build, theme and make accessible: buttons, lists, sliders, text boxes, dialogs.
- No font importer or sanitiser is needed, which removes the riskiest part of theme-pack import. Images and sounds still need re-encoding at import.
- Atkinson Hyperlegible Next (`atkinson-hyperlegible-next-fonts`) and Noto Sans (`google-noto-sans-fonts`) are Fedora 45 packages; each ships with its licence file. Neither font's legibility claims cite independent tests ([open-source fonts](../research/B5-oss-fonts.md)).
- Letters a font family lacks (other languages) fall back to other fonts in the image, which ties into the languages work (series L).
- The Appearance screen needs two theme instances (live and preview).

## Turned down

- Kirigami route C (Hearth's tokens plus a bridge for leftover Kirigami controls), decision 0009's choice: with a fresh shell there are no leftover Kirigami controls to bridge.
- Kirigami route A (replace only, no bridge) and route B (bridge only, keep Kirigami names everywhere): turned down in 0009; moot without Kirigami.
- Fonts allowed in packs, sanitised at import: the lead's first answer in 0009; turned down because font parsing is a known attack route (CVE-2025-27363, a FreeType bug reported as exploited) and a sanitiser adds work and risk.
- Apply-instantly settings: Claude's first proposal in 0009; a bad choice could make the screen being used unreadable.
- Sunrise/sunset switching: needs the box's location, which breaks local-only.
- Other default fonts from the research (Inter, Source Sans 3, Red Hat Text, IBM Plex Sans and others): Claude recommended Atkinson Hyperlegible Next for being made for low vision and packaged in Fedora 45; the lead agreed.
