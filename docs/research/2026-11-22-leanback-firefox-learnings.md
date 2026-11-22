# What Leanback for Dropout learned about Firefox on a TV

*Read by Claude on 2026-11-22, at the lead's request, from the lead's own extension (`firefox-leanback-for-dropout` at `66f6702`: `docs/keymap.md`, `docs/targets.md`, `docs/architecture.md`, `CHANGELOG.md`, its GitHub issues) and the private Leanback Dev Probe. Facts for Hearth's web apps and its TV-navigation extension.*

*Tags: **[T]** = tested by the lead, as recorded there or as the lead says; **[L]** = written in Leanback's docs, no test recorded; [M] = Claude's memory.*

## What a remote's buttons arrive as in Firefox

A typical air mouse, as Firefox's `event.key` sees it. The lead has confirmed the air mouse's buttons work in Firefox while testing Leanback [T, the lead, 2026-11-22].

| Button | Linux key | In Firefox | Notes |
|---|---|---|---|
| Arrows | `KEY_UP` … | `ArrowUp` … | |
| OK | `KEY_ENTER` | `Enter` | Whether OK clicks, sends Enter, or both in mouse mode: open (Leanback's check T8) [L] |
| Back | `KEY_BACK` | `BrowserBack` | Some remotes send `Escape` or `GoBack` instead [L] |
| Home | `KEY_HOMEPAGE` | `BrowserHome` | A TV shell may take it first, so Firefox never sees it [L] |
| Menu | `KEY_COMPOSE` | `ContextMenu` | Opens Firefox's own context menu unless the page cancels the keyboard-made `contextmenu` event, in every frame including the video player's [L, changelog fix] |
| Volume ± | `KEY_VOLUMEUP` / `DOWN` | `AudioVolumeUp` / `Down` | The desktop's [L] |
| Power, mouse-mode switch | nothing reaches the computer | | [L]; matches bifrost's notes on the WeChip W1 |

## Keys Firefox keeps for itself

- **Back can't be stopped by a page.** Cancelling `BrowserBack` with `preventDefault()` doesn't stop Firefox's own history-back, on the page or with the video player focused. [T, checked with the Dev Probe's "Block Back" switch]
- Firefox's Back skips history entries nobody interacted with (`browser.navigation.requireUserInteraction`, on by default), so an entry a page pushes as it loads gets passed over. [L]
- **Esc leaves the page's full screen** (the Fullscreen API). Leanback avoids it by making the video fill the window with CSS instead. [L]
- From Firefox 147 a page can read its own history list (the Navigation API); before that, an extension has to count steps. [L]

## The system's on-screen keyboard

- On bifrost, Plasma's on-screen keyboard opened over Leanback's own keyboard. [T, [issue #36](https://github.com/inklikeurine/firefox-leanback-for-dropout/issues/36)]
- The fix: the text boxes Leanback types into say they take no typing through the system, with `inputmode="none"` and the CSS `ime-mode: disabled`, so the system keyboard stays shut while a real keyboard still types. [L, changelog; whether it was re-checked on the box isn't recorded, and bifrost's matching issue is still open]

## Video and sound

- Firefox blocks autoplay with sound. No extension can lift it: there's no WebExtension API for it, and key presses an extension makes carry no "user activation". Only the profile can: `media.autoplay.default = 0` in `user.js`, or a policy. [L]
- Firefox says it can play H.264 even when it can't (Fedora's cut-down FFmpeg). Leanback checks by decoding one tiny H.264 frame when the player opens. [L; the failure itself is [T] in bifrost]
- Hardware decoding pref for a kiosk profile: `media.ffmpeg.vaapi.enabled = true`. [L]

## Closing, exits and memory

- A page's own `window.close()` is usually refused; the extension's background script closes the tab (`tabs.remove`) or the window (`windows.remove`). In kiosk mode, closing the last tab ends Firefox and drops back to the shell. [L]
- Set the kiosk profile's home page to the site, so a stray `BrowserHome` is harmless. [L]
- One kiosk profile takes about 0.5–1 GiB of RAM. [L, no measurement recorded]

## Other input

- **Game controllers** reach Firefox only through the Gamepad API, not as key presses, and only after a button is pressed while the page is in front. [L, [issue #57](https://github.com/inklikeurine/firefox-leanback-for-dropout/issues/57)]
- **Pointer drift:** focus must never follow the pointer, and site tooltips that pop up on hover get hidden, so a drifting air mouse doesn't move the selection or litter the screen. [L]
- **Held buttons:** a remote that sends many quick presses instead of one held press is treated as a hold: a press within 0.15 s of the same button's release carries it on. OK, Back and Menu never repeat. [L]

## Native messaging

Leanback's parked "sign in from your phone" design uses the same channel Hearth plans for Back: Firefox starts a small helper program (Python, standard library only) through native messaging, asked for as an optional permission only when the feature is turned on. [L, [issue #61](https://github.com/inklikeurine/firefox-leanback-for-dropout/issues/61)]

## Not confirmed

- Leanback's setup checks T1 and T3–T8 have no recorded answers: Home taken by the shell with Firefox still running, the Menu key in kiosk mode, closing in kiosk mode, hardware decoding, the login surviving a reboot, pointer drift, OK in mouse mode.
- Whether `inputmode="none"` also keeps a keyboard shut that works through text-input v3, as Hearth's would (Plasma's used an older protocol).
