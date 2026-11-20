# Tested with Hearth: how apps get rated

*Exploration note, 2026-11-18. Claude's proposal; nothing here is decided.*

## What it is

A public page in Hearth's docs, with the same data shown in the shell's "add an app" screen. It tells a user before installing whether an app or web app works on a TV with a remote. Each entry also carries the app's adapter, which makes the app follow Hearth's global key map (Back always means back, even if the app needs Esc for it). The rating and the adapter come from the same test record.

It covers:

- Apps: Flatpaks from Flathub, plus whatever ships in the image.
- Web-app definitions: YouTube, Twitch, Dropout, and user-built ones that get submitted.

A "Tested hardware" page uses the same process and record format, with different checks.

## How others do it

| Project | How it rates | What to borrow | What to avoid |
|---|---|---|---|
| Steam Deck Verified (Valve) | Four levels: Verified, Playable, Unsupported, Unknown. Verified means passing all four areas: input, display, seamlessness, system support | The four-area checklist, and an "Unknown" level for untested apps | Valve tests privately; we publish every test record |
| ProtonDB (community) | Platinum, Gold, Silver, Borked. Reporters used to pick a tier, which was inconsistent; since 2019 they answer factual questions and the tier is calculated from the answers | Factual yes/no questions with a calculated rating; reporters never pick the rating | Ratings that never expire |
| AppStream (the app-description format Flathub uses) | Apps can declare the inputs they support, including `tv-remote` and `gamepad` | Read it as a hint; our test result wins | Trusting it: it's self-declared |

## Rating levels (proposed)

| Level | Meaning | Shown as |
|---|---|---|
| Great on Hearth | Everything works with a remote, out of the box | Green |
| Works with tweaks | Usable after a one-time step (sign in with the phone keyboard, change one app setting). The entry says what | Yellow |
| Doesn't work | Can't be used with a remote, or breaks Hearth (stray windows, can't get home) | Red; hidden from the shell's app screen unless developer mode is on |
| Not tested | No current test record | Grey |

## The checklist

Four areas, modelled on Steam Deck Verified. Each item is a yes/no question; the level is calculated from the answers.

1. Remote
   - Every main function is reachable with arrows, OK, Back and Home only.
   - Back does what a viewer expects on every screen: closes the menu, then goes back a page, then leaves the app.
   - Home always returns to Hearth.
   - No mouse needed.
   - Text entry works (on-screen keyboard or phone typing).
2. Screen
   - Text readable from 3 m (10 ft) at 1080p.
   - Fits the screen: no cut-off edges, no tiny windows.
   - Clear focus highlight.
3. Seamless
   - Opens full screen, straight from the tile.
   - Returns home cleanly: no leftover window, no prompt the remote can't answer.
   - No pop-ups the remote can't close (password save, notifications, update nags, keyring prompts).
   - Survives sleep and wake, and the weekly restart.
4. System
   - Runs on the floor hardware (4 GB box) within its RAM budget.
   - Video uses hardware decoding where the hardware supports it.
   - Sound goes to the TV.
   - Local only: in a recorded session, it contacts only its own service and nothing on the never list.
   - Has a known update path (Flathub, or a pinned version in Hearth's channel).

Facts shown on every entry, never used to block it:

- licence
- what it's built on ("Built on Chromium" is shown as a fact; users are free to install it)
- whether it needs an account
- whether it uses Google's copy protection

## The process

1. Nominate. Anyone opens a GitHub issue form: app name, Flathub ID or URL, why. The lead can nominate directly.
2. Triage (lead, 5 minutes). Is it a TV-use app? A duplicate? Does it need hardware we don't have?
3. Automatic checks (CI, in a virtual machine):
   - it launches full screen and returns home
   - RAM use on the floor-hardware profile
   - an egress recording (every connection during a scripted session) compared with the allowlist
   - AppStream input declarations read in as hints
4. Remote walkthrough (lead, on real hardware), handed over as numbered steps, expected result, what to report. 10–20 minutes per app. The adapter is written at the same time.
5. Record. One data file per app in the repo, for example `catalog/apps/com.moonlight_stream.Moonlight.toml`, holding:
   - the app's version, Hearth version, hardware tier, date and tester
   - every checklist answer
   - notes
   - the adapter
6. Publish. The docs page and the shell's app screen are both generated from those files.
7. Re-test when:
   - a new major version of the app comes out
   - a Hearth release changes input or the compositor
   - a user reports breakage
   - the record turns 12 months old; it then falls back to "Not tested", with the old result shown as history

## Community reports

The box never sends a report. A user who finds a problem picks "Report how this app works" in the app's menu. The TV shows a QR code holding only the app ID, the app version and the Hearth version. Scanning it opens a GitHub issue form on the phone with the checklist questions. A report changes the rating only after the lead confirms it.

## Open questions

- Is 12 months the right expiry?
- Should "Works with tweaks" apps show in the shell by default?
- Once Hearth has contributors, who can test: only the lead, or trusted testers with their own hardware tier?
- Should web-app definitions submitted by users be listed, or kept as personal recipes?

## Sources

- Steam Deck Verified criteria: https://steamdeck.com/verified
- ProtonDB ratings revised: https://boilingsteam.com/protondb-ratings-revised/
- AppStream device and input metadata: https://blog.tenstral.net/?p=1969
- AppStream input kinds, including TV remote: https://world.pages.gitlab.gnome.org/Rust/libappstream-rs/git/docs/libappstream/enum.ControlKind.html
- Flathub metainfo guidelines: https://docs.flathub.org/docs/for-app-authors/metainfo-guidelines
