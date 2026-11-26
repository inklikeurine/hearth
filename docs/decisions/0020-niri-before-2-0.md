# Decision 0020: niri replaces labwc before 2.0.0

Related: [labwc for 1.0.0](0015-labwc-for-1-0.md), which stands unchanged; [Heavy accessibility in 2.0.0](0019-heavy-accessibility-in-2-0.md); design doc: Session setup, Theme and controls, System menu, On-screen keyboard; [shell spike results](../research/2026-11-22-shell-spike-results.md).

## Context

The lead wants blur behind Hearth's menu and the tvOS-style single-row keyboard, over apps too, and never across the whole screen. Over the home screen, Qt can blur it, because everything behind the panel is Hearth's own. Over an app, only the compositor has the app's pixels, so only the compositor can blur them. labwc can't.

The standard way to ask for this is the ext-background-effect-v1 protocol: a surface asks for a set area behind it to be blurred, and the compositor applies the blur [C, https://wayland.app/protocols/ext-background-effect-v1].

## Decision (the lead, 2026-11-25)

- niri replaces labwc as Hearth's compositor before 2.0.0. labwc stays for 1.0.0, so [labwc for 1.0.0](0015-labwc-for-1-0.md) stands.
- In 1.0.0, the menu and the keyboard ask the compositor for blur through ext-background-effect-v1. When the compositor doesn't offer it, they fall back to a solid tint. labwc doesn't offer it, so 1.0.0 gets the tint. The tint also serves "Reduce transparency", which is already in the 1.0.0 accessibility set. On niri, the blur appears with no change to the shell.
- No window open or close animations: apps open instantly. Movement and scrolling animations for the menu and the keyboard are wanted. Qt draws those inside Hearth's own overlays, so they don't depend on the compositor.
- The lead asked for two additions. Both are Claude's proposals in substance: the niri spike below, in the roadmap's 2.0.0 milestone, and a list of the shell's ties to labwc in the design doc's Session setup section, kept current so the move stays cheap.

## Facts

From the 2026-11-25 research (there is no separate research note):
- Support for ext-background-effect-v1 [S, wayland.app's support tables]:
  - yes: niri 26.04, KWin 6.7, Mutter 51, COSMIC 1.11, Hyprland 0.57;
  - no: labwc 0.20.2, Wayfire, sway, Mir, Weston, cage, river.
- The protocols the shell already uses are layer-shell, wlr foreign-toplevel, input-method v2, virtual-keyboard v1 and text-input v3. wayland.app lists niri 26.04 with all of them [S, wayland.app]. The input-method v2 and text-input v3 rows came from cut-off pages, so those two are less certain.
- niri's blur shipped in 26.04 (April 2026). It applies to layer-shell surfaces, either because the surface asks through the protocol or through a niri config rule. It follows any shape the surface asks for [C, https://github.com/niri-wm/niri/wiki/Window-Effects].
  - The default "xray" blur shows only the wallpaper, so it's no use over an app.
  - Real blur of what's underneath is "experimental". It is recomputed whenever the content underneath changes, and it disappears during window open and close animations.

Checked by Claude on 2026-11-25:
- niri can switch off all its animations with `off` in its `animations` block, or one animation at a time by putting `off` inside it [C, https://github.com/niri-wm/niri/wiki/Configuration:-Animations]. The page has no literal example of `window-open { off }`, but its rule covers that form.
- Fedora 45 packages niri 26.04-2.fc45 in its main repo [C, the dev box's package cache, `dnf -C info niri`]. Nothing was installed.
- niri's contribution rules forbid LLM-made contributions: "All pull request code (including tests), issue descriptions, comments, public reviews, etc. must be human-written" [C, https://raw.githubusercontent.com/niri-wm/niri/main/CONTRIBUTING.md]. Claude's reading: under Hearth's upstream rule, nothing Claude writes goes to niri. Any bug report or change is the lead's own, written by hand.
- niri has had basic screen-reader (Orca) support since 25.08 [C, https://github.com/niri-wm/niri/wiki/Accessibility]. That matches what [Heavy accessibility in 2.0.0](0019-heavy-accessibility-in-2-0.md) asks of the 2.0.0 compositor. Untested.

## Found while listing the labwc ties (Claude, 2026-11-25)

These don't touch the decision, but they make the move bigger than a config swap. Details are in the design doc's Session setup list.
- **No minimize.** niri ignores foreign-toplevel's minimize request and never reports a window as minimized [C, `SetMinimized => ()` in https://raw.githubusercontent.com/niri-wm/niri/main/src/protocols/foreign_toplevel.rs]. Hearth goes home by minimizing the app, so on niri going home needs another way. Not checked how.
- **Release key bindings may be newer than 26.04.** niri's wiki marks bindings that fire on release as "Since: next release" [C, https://github.com/niri-wm/niri/wiki/Configuration:-Key-Bindings]. Hearth fires Back, Home and Menu on release, so a held key fires once. `repeat=false` (since 0.1.8) fires once on press instead.
- **Headless tests.** niri's headless backend is for niri's own tests and has no command-line switch [C, https://raw.githubusercontent.com/niri-wm/niri/main/src/backend/headless.rs]. Also, "Without a screen, niri won't give focus any window" [C, Accessibility page]. Hearth's session tests run labwc with no screen, so they would need another way to run.

## The spike (Claude's proposal)

In the roadmap's 2.0.0 milestone.

Dev box or VM:
- Re-run the labwc shell spike's checks on niri ([shell spike results](../research/2026-11-22-shell-spike-results.md)).
- One full-screen app at a time, through niri's window rules.
- App open is instant with niri's animations off.
- The blur area follows the menu as it slides in.
- Added by Claude after reading niri's source: going home and back without minimize, and Back, Home and Menu firing once when held, with the bindings Fedora's niri version has.

Real hardware (the OptiPlex, then the 4 GB box):
- Blur behind the keyboard strip over playing 1080p video, with no dropped frames.

If the spike fails, the lead decides again (Claude's proposal).

## Consequences

- 1.0.0 builds the blur request into the menu and keyboard from the start, with the tint as fallback, so the 2.0.0 move needs no shell change for blur.
- Claude's note: over an app, niri's blur has to be the experimental kind, since xray shows only the wallpaper. niri recomputes it whenever the content underneath changes, which for video means every frame. Whether a floor box keeps up is the spike's real-hardware check. Choosing that kind of blur is a niri setting, not a shell change [M: the exact setting wasn't checked].
- niri's blur disappears during window open and close animations. With those animations off, as the lead wants, that shouldn't come up. The spike checks it.
- The shell's ties to labwc are listed in the design doc's Session setup section and kept current.

## Open

- Whether Qt 6.11 speaks ext-background-effect-v1 itself. If it doesn't, Hearth adds it as a small Qt Wayland extension, the way layer-shell-qt adds layer-shell [M].
- Whether niri's key bindings take keys before the on-screen keyboard's input-method grab, as labwc's do. Not checked.
- Whether niri ends when the program it was told to start ends, as Hearth's session needs. Not checked.

## Turned down

- Staying on labwc for 2.0.0: it has no compositor blur.
- Hyprland: isn't packaged in Fedora 45.
- COSMIC: has no wlr foreign-toplevel.
- KWin 6.7: has no input-method v2 or virtual-keyboard v1.
- Mutter (GNOME): has no layer-shell for overlays.
