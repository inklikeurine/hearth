# The shell, for contributors

The shell is the program that draws Hearth: the home screen, the menu, the overlays and Settings. This page says where its parts are and how to work on them. What it should do is in the [design doc](design.md#4-shell); what's left is in the [roadmap](roadmap.md).

## Running it

| What | How |
|---|---|
| Every check | `tools/dev/check` (`--fix` reformats) |
| A whole session in a window | `tools/dev/session`, after a build |
| Just the shell, as a window | `build/src/shell/hearth-shell` in the dev box (no app switching: there's no labwc around it) |
| Every control, in both themes | `build/tests/gallery/hearth-gallery [high-contrast]` |
| The session test, with pictures | `tests/session/run.sh build PICTURE_FOLDER` |

In the dev box nothing the shell does reaches your desktop: it uses no message bus, so power buttons, notifications and media keys do nothing there. `src/shell/system/buses.cpp` decides that.

## How a session fits together

1. greetd runs `hearth-session` for a profile ([src/session](../src/session)).
2. That writes the profile's labwc settings (`hearth-shell --write-labwc`), then starts labwc.
3. Inside labwc it starts the on-screen keyboard and the shell, and starts the shell again if it crashes (three times at most).
4. labwc catches Back, Home, Menu, Esc, Backspace and the volume keys from every device and runs `hearth-send`, which passes them to the shell over a socket only this user can open.
5. The shell's `KeyRouter` decides what the key means now; `AppSwitcher` keeps one app in front, through labwc's window-switching protocol.

## Where things are

| Folder | QML module | What's in it |
|---|---|---|
| `src/shell/theme` | `Hearth.Theme` | Theme tokens and the built-in themes |
| `src/shell/controls` | `Hearth.Controls` | Hearth's own controls: buttons, tiles, list rows, toggles, sliders, text boxes, dialogs, icons |
| `src/shell/apps` | `Hearth.Apps` | The installed apps, starting them, the profile's settings file |
| `src/shell/session` | `Hearth.Session` | Windows, app switching, keys from labwc, the menu API, labwc's settings |
| `src/shell/system` | `Hearth.System` | Power, network state, volume, notifications, media players, idle time |
| `src/shell/failures` | `Hearth.Failures` | Failure screens, [error codes](error-codes.md), QR codes |
| `src/shell/overlays` | `Hearth.Overlays` | Windows above apps (layer shell) |
| `src/shell/panels` | `Hearth.Panels` | The status bar, the system menu, notifications, volume, the screensaver |
| `src/settings` | `Hearth.Settings` | Settings and its pages |
| `src/shell` | `Hearth.Shell` | `Main.qml`, which ties them together, and the home screen |
| `src/send` | — | `hearth-send` |
| `src/session` | — | `hearth-session`, labwc's settings, the systemd target, the portal file |

## Rules that aren't obvious from the code

- Every colour, size and duration comes from the theme: `Theme.tokens.…`.
- Focus never follows the pointer; everything works with arrows, Select and Back.
- Every control gets an accessible name and role.
- Every string goes through KI18n: `KI18n.i18n("…")` in QML.
- Every failure gets a screen and an error code, listed in [error-codes.md](error-codes.md); a test compares the two.
- Each feature ships with its settings page and its failure screens.
- Tests wait for things to happen, never for a fixed time: the dev box shares its processor.
- Fedora's Qt drops `console.log`; use `console.warn`.
