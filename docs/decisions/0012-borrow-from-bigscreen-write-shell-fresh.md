# Decision 0012: Hearth writes its own shell and borrows parts from Bigscreen

Related: design doc, sections 2, 5.4, 6.2 and 9.

## Context

The plan was to fork Plasma Bigscreen's interface and remove Plasma. Most of Bigscreen turned out to be Plasma glue:
- The home screen is a Plasma plugin that runs inside plasmashell, so it has to be rehosted anyway.
- `shell`, `lookandfeel` and `envmanager` are Plasma packages and Plasma configuration.
- The settings pages are tied to Plasma. The design doc already planned to rebuild them.
- `webapp-viewer` is built on Qt WebEngine, which is Chromium.
- The input handler types fake keys through a portal that KWin provides, and the compositor may not be KWin (section 5.4).

## Decision

- Hearth writes its own shell (home screen, settings, overlays) in Qt 6 QML.
- It borrows from Bigscreen, with credit:
  - the input handler: game controllers through SDL3, TV remotes through libcec, the device watcher, and the permission rules for the virtual-keyboard device (section 6.2)
  - the link redirector, for "cast a link" (section 7)
  - design ideas from the home screen

## Consequences

- No upstream merges to track, and no cleanup passes that conflict with upstream.

## Turned down

- Keeping the fork: most of it is Plasma code Hearth would delete, and the rest would drift from upstream anyway.
- Taking nothing from Bigscreen: it would mean redoing controller and CEC code that already works.
