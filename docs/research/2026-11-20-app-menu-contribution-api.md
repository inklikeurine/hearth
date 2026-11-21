# App Menu Contribution API (2026-11-20)

An exploration of the idea, not a decision. Labels: [C] primary source, [S] secondary, [M] my reasoning.

## What it might be

When the user presses Menu, they see a single overlay drawn by Hearth. The overlay has two sections:

- **System section** — always present. Settings, power, network, accessibility shortcuts, profile switcher.
- **App section** — shown when an app is open. The app contributes structured data describing what controls it offers. Hearth renders them consistently.

The app never draws into the menu. It sends a small JSON payload over a local IPC channel (D-Bus for native apps, the Firefox extension's native messaging channel for web apps). Hearth owns the presentation, navigation, focus, and permissions.

## Principles?

1. **The OS owns the menu.** The app describes its available controls; Hearth decides where they appear, how they're styled, how focus moves through them, and whether they're shown at all (permissions, parental controls).
2. **Structured data only.** No HTML, QML, scripts, SVG, or embedded graphical surfaces. Only predefined control types and data fields are accepted.
3. **Trusted identity.** The app's name and icon come from its installed manifest (Flatpak metadata, web-app definition), not from the API payload. The app can't lie about who it is.
4. **Rate-limited and bounded.** Maximum menu depth, item count, label length, update frequency, and payload size. The shell enforces all of them.
5. **Request vs. confirmed state.** The app can report its current state ("playing", "paused at 12:34") and request a state change ("pause"). The shell decides whether to honour the request. The app never asserts authority over the system UI.

## Open questions
1. How does the menu handle apps that don't support the API? 
2. How does the menu interact with the on-screen keyboard? The keyboard is a Hearth overlay, not an app concern. The app receives the typed text through a separate channel?
3. Should the menu appear over a web app that's in full-screen video?
4. Does the menu pause the video when it opens?
5. How deep can submenus go?
6. How does the menu handle multiple media sources (e.g., a playlist)? 
What about apps that have no media state (a settings app, a file browser)?
