# Decision 0012: Hearth writes its own shell and borrows parts from Bigscreen

Accepted 2026-11-22 by the lead. Replaces the Agreed line "Fork Plasma Bigscreen's UI and drop Plasma" (2026-11-18). Related: design doc, sections 2, 5.4, 6.2 and 9.

## Context

The plan was to fork Plasma Bigscreen's interface and remove Plasma. Read on 2026-11-22 (repo at commit `fb80653`), most of Bigscreen turned out to be Plasma glue:

- The home screen is a Plasma plugin that runs inside plasmashell, so it has to be rehosted anyway.
- `shell`, `lookandfeel` and `envmanager` are Plasma packages and Plasma configuration.
- The settings pages are tied to Plasma. The design doc already planned to rebuild them (section 10).
- `webapp-viewer` is built on Qt WebEngine, which is Chromium.
- The input handler types fake keys through a portal that KWin provides, and the compositor may not be KWin (section 5.4).
- Upkeep is thin: one volunteer revived Bigscreen in 2025 and has gone back to Plasma Mobile ([E5](../research/E5-compositor-upkeep.md)).

## Decision

- Hearth writes its own shell (home screen, settings, overlays) in Qt 6 QML.
- It borrows from Bigscreen, with credit:
  - the input handler: game controllers through SDL3, TV remotes through libcec, the device watcher, and the permission rules for the virtual-keyboard device (section 6.2)
  - the link redirector, for "cast a link" (section 7)
  - design ideas from the home screen

## Consequences

- The shell's licence is no longer forced. It was GPL because of the fork. Borrowed files keep their own licences:
  - input handler files: `GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL`
  - link redirector: `GPL-2.0-only`

  GPL-2.0-only code can't be combined with GPL-3.0-only code in one program. The link redirector is small and is its own program, so it can stay separate or be rewritten. The lead then chose GPL-3.0-or-later for the shell (2026-11-22, design doc section 2).
- Fedora 45 still holds without Bigscreen. The installer needs systemd 261 ([Installer](0005-installer-boot-profile-sysinstall.md)).
- No upstream merges to track, and no cleanup passes that conflict with upstream.
- Contributions back to Bigscreen follow KDE's AI policy (design doc, section 16).

## Turned down

- Keeping the fork: most of it is Plasma code Hearth would delete, and the rest would drift from upstream anyway.
- Taking nothing from Bigscreen: it would mean redoing controller and CEC code that already works.
