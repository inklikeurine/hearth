# Decision 0015: labwc is Hearth 1.0.0's compositor

Related: design doc, sections 5.4, 6.2, 6.4 and 7; [shell spike results](../research/2026-11-22-shell-spike-results.md); [Heavy accessibility in 2.0](0019-heavy-accessibility-in-2-0.md).

## Context

The compositor is the program that draws every window and passes input to apps. Hearth needs one full-screen app at a time, its own overlays above it (menu, volume, keyboard), switching apps and going home, and its on-screen keyboard typing into every app.

The candidates were KWin on its own, labwc (a small compositor built on the wlroots library), and Hearth as its own compositor (Qt Wayland Compositor). Cage and Weston were already out: neither speaks the keyboard protocols Firefox uses, and neither has an overlay layer (design doc, section 5.4).

The shell spike ran on Fedora 45's labwc 0.20.2, headless in the dev box. Every software check passed (69 of 69, nice): home screen, menu overlay over full-screen Firefox, going home and back, forcing apps full screen, the remote's Back through native messaging, and typing with a test keyboard through input-method v2. 

## Decision

- Hearth 1.0.0 runs on labwc.

## Consequences

From the spike and the research:
- The shell and the keyboard use only standard protocols: layer-shell (overlays), foreign-toplevel (app switching), text-input v3 and input-method v2 with its keyboard grab (typing), virtual-keyboard v1 (keys with no text). Another wlroots-based compositor that offers the same set could take the shell as it is.
- Moving to KWin later would cost more: from memory, KWin offers keyboards only input-method v1, so the keyboard's link to the compositor would be rewritten. Not checked.
- labwc has no screen reader, zoom or colour filters. Those are 2.0.0 features ([Heavy accessibility in 2.0.0](0019-heavy-accessibility-in-2-0.md)); 2.0 needs its own plan for them.
- HDR on labwc needs its Vulkan renderer; the default renderer has no HDR.
- The display settings protocol labwc offers has no TV colour-range setting (design doc, section 10).

## Turned down

- KWin on its own: heavier, pulls in Plasma libraries; its built-in screen reader, zoom and colour filters matter for 2.0.0, not 1.0.0.
- Hearth as its own compositor (Qt Wayland Compositor): Hearth would take on the whole compositor job (display modes, HDR, input devices, keyboard input into apps, screen reader), and it has no layer-shell for overlays drawn by other programs.
- cage and Weston (out before this decision): no text-input v3 or input-method v2, no overlay layer.
- gamescope (likely out before this decision): needs Vulkan, which a lot of floor-class graphics lack or only half-support.
