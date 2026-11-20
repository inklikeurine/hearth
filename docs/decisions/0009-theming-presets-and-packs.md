# Decision 0009: Theming: Hearth-owned tokens, preview-first Appearance, day/night toggle, no fonts in theme packs

Accepted 2026-11-19 by the lead. Related: design doc, sections 2 and 9; [theming research](../research/2026-11-19-theming.md).

## Context

Theming is a high-priority goal, built in from the start. The shell forks KDE Plasma Bigscreen, whose colours come from Plasma's desktop colour scheme. With Plasma removed, Kirigami (KDE's QML toolkit) falls back to the Qt style's colours or its own built-in palette, so Hearth has to own its colours either way. Bigscreen's shared controls and home screen use Kirigami's theme and size names 362 times. Kirigami's colours can be fed from one QML file with no C++; its sizes are read-only from QML.

Font files are a known attack route: CVE-2025-27363, a FreeType font-parsing bug, was reported as exploited in real attacks. The lead first allowed fonts in theme packs, then ruled them out the same day.

## Decision

1. Kirigami route C. Hearth's own theme drives the shared controls and the home screen directly, plus a small QML file that passes Hearth's colours to any Kirigami controls that remain.
2. No custom fonts in theme packs. Hearth ships a set of open-source fonts in the OS image, and theme authors pick a family from those.
3. Appearance settings are preview first, then apply.
4. Automatic day/night switching is user-settable: a toggle, off by default. On shows a Day theme and a Night theme dropdown, pre-filled from the theme's own light and dark modes unless the user has set them, switching at two fixed times the user sets (no sunrise/sunset, which would need the box's location). Off: one theme all the time; a theme with two modes shows its author's default mode. No playback check.
5. The preset set and default look are scoped from scratch (research series A and B in the lead's log).
6. Theme packs are data only, never code (as in the design doc).

## Consequences

- The fork's first job is moving shared controls and the home screen onto Hearth's theme names, before any new screen is written; new screens copy whatever names existing screens use.
- No font importer or sanitiser is needed, which removes the riskiest part of theme-pack import. Images and sounds still need re-encoding at import.
- The open-source fonts in the image need choosing (research prompt B5), each shipped with its licence file.
- Letters a font family lacks (other languages) fall back to other fonts in the image, which ties into the languages work (series L).
- The Appearance screen needs two theme instances (live and preview).

## Turned down

- Kirigami route A (replace only, no bridge): any leftover Kirigami control would show Kirigami's built-in colours.
- Kirigami route B (bridge only, keep Kirigami names everywhere): motion speed, focus style and text scale aren't Kirigami concepts, and Kirigami's sizes need a C++ plugin on internals that can change between releases.
- Fonts allowed in packs, sanitised at import: the lead's first answer; turned down because font parsing is a known attack route and a sanitiser adds work and risk.
- Apply-instantly settings: Claude's first proposal; a bad choice could make the screen being used unreadable.
- Sunrise/sunset switching: needs the box's location, which breaks local-only.
