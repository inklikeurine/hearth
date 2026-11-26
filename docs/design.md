# Hearth design
How to read it:
- **Decided** lines are the lead's. Each names its decision record where one exists; otherwise it's an "Agreed" line that has no record yet.
- **Waiting on the lead** lines are Claude's proposals. Nothing in them is decided. The [roadmap](roadmap.md) grid shows which piece of work each one blocks.
- Answered questions are deleted, not marked done. Git keeps the history.

## 1. Goals (the lead's)
1. Easy to install and keep running. Like a basic fridge: it works, and updates happen without the user noticing.
2. Replaces a smart TV. TV remotes as first-class input with sensible defaults, every app usable with a remote or controller, all settings reachable from the remote.
3. Local only. No telemetry, no collecting user data. Users can configure their box however they like.
4. Secure. AI makes attacks faster and cheaper, so security is a priority from day one.
5. Well documented. Easy-to-follow code, easier docs.
6. A plan for thorough agentic review that fits a solo developer working with AI (to be researched).
7. Anything the co-designer spots.

### Nice to haves
The following features are classified as "nice to have".
1. Phone pairing (as a remote and as a connected device), in 2.0.0
2. web apps as first-class apps (in 1.0.0; work starts near the end, after the shell and most of the UI are settled)

## 2. Rules for every part

### How we work
- **Features and their screens together:** each feature is built with the settings page that controls it and its failure screens (the lead, 2026-11-24).
- **Design doc and roadmap**: this doc is a component map with a short spec per component; facts live in research notes, linked. Milestones are GitHub milestones, used to track how complete a feature is and whether it's ready to ship. Milestones go part by part. Every milestone gets broken into tasks, not necessarily now.
- **Product code**: the roadmap grid shows which open decision blocks which piece of work. A piece of work can start once nothing in the grid blocks it, and only when the lead asks.
- **Development:** In a Fedora 45 distrobox on the lead's machine ([dev environment](dev-environment.md)). Spike code lives on a local-only `spikes` branch that is never pushed.
- **The repo:** uses GitHub's free features: milestones, labels, Actions, Projects, security and code-quality tools, bots.
- **Parked**: hosted code reviewers until we're ready to code; branch protection for `main` after the code reviewers; the lighter-Firefox review closer to 1.0; a boot-time target, later.

### Apps
- The plan doesn't name specific apps or web services. Native apps are developed in their own repos, later; which apps to include isn't decided now (the lead, 2026-11-24).

### Licence, borrowing, upstreams

- **Licence:** GPL-3.0-or-later. GPL-2.0-only code, such as Bigscreen's link redirector, stays a separate program or is rewritten.
- **Borrowing:** borrow freely from open-source projects, with credit.
- **Upstream etiquette:** Before contributing anything to upstream (code, a bug report, an issue), look for an AI policy. Follow it if there is one; if there isn't, assume they don't want AI-assisted input and keep our changes in our own repo. This only needs to be looked into if we are going to submit something upstream. Not beforehand.
- **No Chromium:** our development doesn't build on or ship Chromium or Electron. Users may install Chromium-based apps.

### Local only
> Hearth doesn't phone home or send back any personal data. We neither need or want it. Every kind of connection is named, listed and switchable in Settings → Privacy and network activity.

- **Network defaults:** AdGuard Home keeps its own defaults, upstream DNS included. Time source: the base image's default. Per-app audits are not the goal.
- **Apps the user installs:** Per-app audits are not the goal.

### Security

- Updates are the main defense: signed images, every Firefox security release triggers an image, urgent fixes get an out-of-cycle image.
- Fedora's stock kernel, all CPU security mitigations on. Kernel lockdown off. Secure Boot off by default.
- Supply chain: pin versions for everything pulled from outside, with a checksum for files from outside Fedora (the lead, 2026-11-24); a new version reaches boxes only through a release the lead approves.

### Hardware
- **Floor:** 4 GB RAM, x86-64: Intel 7th gen (Kaby Lake) or newer, AMD Raven Ridge-class APUs (2018) or newer, NVIDIA Turing (GTX 16 / RTX 20) or newer. NVIDIA Pascal (GTX 10) is legacy, its support ending October 2028. Raspberry Pi 4 is a design goal only.
- **Firmware:** UEFI only; boxes in legacy (BIOS) mode are switched to UEFI first, and the install guide says how ([UEFI only](decisions/0014-uefi-only.md)).
- **Optional hardware:** on-board Wi-Fi, Bluetooth, USB infrared receivers, gamepads and HDMI-CEC are recommended and supported by default, never assumed present. Every core path (install, setup, recovery, settings, updates) works without them; features that need one appear only when it's there, or say what's missing ([Optional hardware](decisions/0008-optional-hardware-supported-never-assumed.md)).
- **RAM budget** on a 4 GB box: system and DNS filter 300–400 MB, compositor 80–150 MB, shell 150–250 MB, one Firefox web app 600 MB–1.2 GB, native video playback 150–300 MB. One web app open at a time; going home closes it after a short grace period; zram on.
- Test boxes: a Dell OptiPlex 3080 Micro and a Lenovo ThinkCentre M910q Tiny (at the floor) ([bifrost: test boxes and TVs](research/2026-11-22-bifrost-optiplex-and-tv.md)).

### When things break
- Solo projects forget the broken state, so Hearth designs it first. Every failure has a screen: one sentence, what to do next, an error code and a QR code.
- Every error code is human readable (`HEARTH-UPDATE-012`), and help pages live on the project's GitHub wiki.
- The QR code carries only the error code and the Hearth version. The box sends nothing.
- Process rule: every piece of work has a "what it looks like when it breaks" part and isn't done until its failure screens exist.

### Accessibility
([Heavy accessibility in 2.0.0](decisions/0019-heavy-accessibility-in-2-0.md)).
- Heavy features are slated for 2.0.0: screen reader, color filters, dialogue boost. Zoom and a separate simple layout are dropped.
- Cheap features slated for 1.0.0: text size, high contrast, never color as the only signal, mono audio and balance, slower key repeat and ignoring double presses, reduced motion, fewer rows and bigger tiles, reduce transparency (added by the lead, 2026-11-25; no record yet)
- Every new screen is checked with the high-contrast theme before it's done.
- Every control gets an accessible name and role (Qt's `Accessible.name`, `Accessible.role`), so the 2.0.0 screen reader doesn't reopen every screen.

### Languages

English only until 1.0.0, scoped so 1.0.0 doesn't block other languages. Hearth's text goes through KDE's translation system (KI18n). The image ships every system language (`glibc-all-langpacks`), the Chinese, Japanese and Korean fonts and Firefox's language packs: easy translations and little work matter more than disk space ([Bigscreen translations](research/L1-bigscreen-translations.md), [system locales](research/L3-system-locales.md)).

## 3. Component map

Each part is a milestone in the [roadmap](roadmap.md).

| Part | Components | Runs as |
|---|---|---|
| Shell | Session setup · Theme and controls · Home screen · Overlays · Key handling · System menu · On-screen keyboard · Notifications · Failure screens | The logged-in profile's Unix user |
| Web apps | Web-app definitions · Firefox setup · `hearth-web-bridge` | The profile's user |
| OS image and releases | Repo and code checks · Image build · Boot and updates · Installer · Recovery and diagnostics · Network and permission rules · CI · Publishing | Build: GitHub Actions or the lead's machine. Box: system |
| Profiles | Login screen · Profiles service · DNS filter · Screen-time keeper · Parental-control screens | System services, the `greeter` user, screens in the session |
| Hardware | Hardware check · Hardware add-ons · Gamepads and HDMI-CEC · Power and sleep | System |
| Settings and setup | Settings app · First-boot wizard · Apps | Screens in the session |
| Docs and help | Help pages · Install guide · User and contributor docs | — |
| Phone (2.0.0) | Phone features | The profile's user |

How the parts talk:

```
greetd ──starts──▶ Login screen ──picks a profile──▶ greetd starts the session
                                                      │
Session (profile's user):  labwc ── Shell ── On-screen keyboard
                              │       │  ▲
                 apps (Flatpak, Firefox web apps)   │ D-Bus
                              │                     │
          hearth-web-bridge ◀─native messaging─▶ Shell
                                                    │
System: Profiles service · DNS filter · Updates (sysupdate) · Hardware check · Add-ons
         (privileged actions checked by polkit rules)
```

## 4. Shell

Hearth writes its own shell in Qt 6 QML, with no plasmashell and no desktop session underneath. It borrows parts from Plasma Bigscreen with credit: the input handler, the link redirector, design ideas ([Borrow from Bigscreen](decisions/0012-borrow-from-bigscreen-write-shell-fresh.md)). The shell spike proved the core loop in the dev box ([shell spike results](research/2026-11-22-shell-spike-results.md)).

### Session setup

Does: starts labwc, the shell and the keyboard for a profile; key bindings; window rules that force apps full screen.
Talks to: labwc, greetd's session.

Decided:
- labwc is the compositor for 1.0.0 ([labwc for 1.0.0](decisions/0015-labwc-for-1-0.md)).
- `wlr-protocols-devel` is approved as a dependency, for the window-switching protocol.

Detail: why labwc and not KWin, Weston, cage or our own compositor; Fedora 45's labwc protocols ([design doc detail, 5.4](research/2026-11-23-design-doc-detail.md), [labwc protocols on Fedora 45](research/2026-11-22-labwc-protocols-fedora45.md)).

### Theme and controls

Does: every colour, size, font and timing in one place (theme tokens); Hearth's own TV controls (tile, button, list, grid, slider, toggle, text box, dialog) and focus ring.
Talks to: every screen; web apps take the focus highlight and overlay colours from it.

- Theming is high priority, built in from the start. Hearth's own controls and theme tokens; preview-first Appearance; day/night toggle (off by default, fixed times); no fonts in theme packs ([Theming](decisions/0016-theming-own-tokens-no-kirigami.md)).
- Fonts: Atkinson Hyperlegible Next by default, Noto Sans for wider coverage; a settings page to change fonts, designed with the settings app.
- Theme packs are data only (tokens, wallpapers, icons, sounds), never code. Layout skins are QML, so developer mode only, after 1.0.0.

Agreed for now (the lead, 2026-11-24); tested on a TV before 1.0.0 ships:
- Text sizes: smallest text 24 px at 1080p, body around 32–36 px, checked from a sofa ([TV text size](research/A4-tv-text-size.md)).
- TV-safe margins: 54–60 px top and bottom, 80–96 px at the sides at 1920×1080 ([TV layout and safe area](research/B3-tv-layout-safe-area.md)).
- Motion: about 200 ms with Material 3's standard easing as the starting point; reduced motion swaps slides and zooms for fades ([TV motion](research/B4-tv-motion.md)).
- Focus never follows the pointer, on every screen (a drifting air mouse must not move the selection).
- Ship the "variable" font packages (one file for all weights). Noto Sans has one; Fedora 45's Atkinson Hyperlegible Next is one file per weight.
- An automatic test checks every built-in theme against WCAG AA contrast.

Agreed (the lead, 2026-11-25):
- Blur on the home screen only; the menu never blurs open apps. Qt draws Hearth's own blur, so labwc needs nothing (labwc has no compositor blur).
- The lead's likes and dislikes from 11 reference screenshots, and the round 1 mockups: [visual references](research/V1-visual-references.md).

Detail: [theming research](research/2026-11-19-theming.md), [TV design principles](research/B1-tv-design-principles.md), [TV focus and navigation](research/B2-tv-focus-navigation.md), [open-source fonts](research/B5-oss-fonts.md), what other platforms offer ([design doc detail, 9.1](research/2026-11-23-design-doc-detail.md)).

### Home screen

Does: the desktop the apps sit on: installed apps in rows, launching and going home, the background, celebrations and particles.
Talks to: the app list (desktop files, Flatpaks, web-app definitions), labwc's window switching.

- One full-screen app at a time. The installed apps in rows the user arranges, not sorted into categories; one row shows at a time with a little of the next row peeking in; a rows-and-columns setting; slick animations; a user-set background; celebrations and particle effects on the home screen only (the lead's wants, 2026-11-22).
- The home screen is a desktop the apps sit on, not an overlay (the lead: "rebuild as a desktop", 2026-11-22).
- Arrange apps with Menu → Move, arrows, Select (long-press "jiggle" isn't reliable on NEC infrared remotes).
- Each profile keeps its own app order; new apps go at the end; a kid profile shows only its allowed apps.
- Still images only as backgrounds in 1.0.0.
- Particles: no more than three flashes a second ([WCAG numbers](research/A3-wcag22-numbers.md)).
- Theme packs can supply art for app tiles (the lead, 2026-11-25). Tile art as a whole needs scoping, including whether art is ever fetched online (undecided) and the lead's "default tile" idea: the app's icon, a name the user can change, and a background colour picked automatically ([visual references](research/V1-visual-references.md)).

### Overlays

Does: the status overlay (clock, battery if any, profile, notification bell, network state, profile switcher and power) and the volume overlay; burn-in protection; the screensaver.
Talks to: PipeWire (volume), NetworkManager, the notification service, the profiles service.

Decided:
- The clock and other fixed elements get burn-in protection: they dim or shift a few pixels over time, and a screensaver starts when the screen sits idle.
- Internet status shows the network state (wired, Wi-Fi, none). "No internet" shows only when something actually fails; no new connection is added to check (2026-11-22). How: AdGuard Home answers "server failure" when the connection is down (tested, 2026-11-24).
- Screensaver delay is user settable with a 2 minute default ([OLED burn-in](research/A6-oled-burn-in.md)).

### Key handling

Does: turns Back, Home, Menu, Esc and Backspace from every device into what they mean right now; passes typing through.
Talks to: labwc key bindings, the on-screen keyboard, `hearth-web-bridge`.

- Air-mouse, infrared and Bluetooth remotes are first-class and all ship with default key maps; HDMI-CEC is a bonus. None is assumed present. The "press each button" learning wizard is optional for the end user.
- One global key map: a button means the same thing in every app; making every app obey it is Hearth's job.
- Keys are handled by what they are, not by which device sent them. labwc's key bindings catch Back, Home, Menu, Esc and Backspace from every device and the shell decides what each means. Esc is Back. Backspace deletes in a text box and is Back anywhere else. Arrows and Select drive the on-screen keyboard while it's shown. Anything that types goes into the text box (2026-11-22).
- Held keys: Back, Esc, Home and Menu fire once, on release; only Backspace in a text box and arrows in the on-screen keyboard repeat (2026-11-22).
- Back in native apps (app adapters): later.

Detail: Bigscreen's input handler compared, what the air mouse sends ([design doc detail, 6–6.2](research/2026-11-23-design-doc-detail.md)).

### System menu

Does: the overlay the Menu button opens: a system section always, an app section when the app in front has contributed items. The Menu Contribution API is how apps add items.
Talks to: apps over D-Bus, web apps through `hearth-web-bridge`, MPRIS for play and pause.

- Hearth owns the menu. Apps add items through a Menu Contribution API, in 1.0.0, used by Hearth's own screens and by web apps. A public SDK for app makers comes after 1.0.0.
- The lead's design notes: apps send structured data only; name and icon come from the installed manifest; Hearth enforces limits and owns presentation; the app reports state and requests changes; no toolkit or compositor dependency ([App Menu Contribution API](research/2026-11-20-app-menu-contribution-api.md)).

Agreed for now (the lead, 2026-11-24):
- Borrow MPRIS so every media app gets play and pause; GTK's exported actions for request-versus-confirmed state; Qt Application Manager for ideas only.
- Web pages never talk to the menu; the bridge contributes items per site.

Agreed (the lead, 2026-11-25):
- The menu slides in from the left edge, docked full height; a setting moves it to the right edge.
- Two ways to show it, for now while exploring: over the home screen it's drawn inside the home screen's own window (so it can blur what's behind it); over apps it's an overlay with a solid tint. To be tried in dev.

### On-screen keyboard

Does: types into every app, driven by remote, gamepad or keyboard.
Talks to: labwc through input-method v2 and virtual-keyboard v1; apps through text-input v3.

- Types into every app, including Flatpaks the user installs.
- Designed from scratch now the compositor is chosen. Layouts: QWERTY grid, alphabet grid, single row, gamepad wheel. Themeable.
- Layout notes (the lead, 2026-11-25): the single row is the tvOS style and may need a compositor that can blur part of the background; the QWERTY grid may end up as an alternate layout of the alphabet grid, coming in from the side as it does rather than up from the bottom; the gamepad wheel is in scope.
- Word suggestions, kept per profile, with an off switch (2026-11-22).
- A layout per kind of field (text, email, password, number), as Gboard does; no suggestions in password fields.
- The word-suggestion engine (a technical pick left to Claude). Claude's pick, 2026-11-24: a small one of our own that completes the word being typed and ranks by what the profile has typed before, with spelling fixes and the word list from hunspell. The usual engine, presage, isn't in Fedora 45, and Fedora's Stevia uses hunspell only (checked in the dev box). Open: how to rank words for a profile that hasn't typed anything yet.

Agreed for now (the lead, 2026-11-24); tested on a TV before 1.0.0 ships:
- Build it as a Qt QML keyboard from plasma-keyboard's layouts and navigation, talking input-method v2 the way Stevia does.
- Honour a web page's `inputmode="none"` through the bridge, since Firefox doesn't pass it on.

Detail: [existing keyboards](research/K3-existing-keyboards.md), [TV keyboard layouts](research/K4-tv-keyboard-layouts.md), [Wayland keyboard protocols](research/K1-wayland-keyboard-protocols.md), [apps and text input](research/K2-apps-text-input.md), parts to borrow ([design doc detail, 6.4](research/2026-11-23-design-doc-detail.md)).

### Notifications

Does: Hearth's own notification service (apps can freeze without one), the bell, short pop-ups.
Hearth ships its own notification service.

### Failure screens

Does: one failure-screen design, the list of error codes, a QR code to each code's wiki page. See "When things break" above.

## 5. Web apps

### Web-app definitions

Does: a small file per web app: name, icon, address, user agent, extensions, adapter (normally the bridge), which profiles may use it. Which ones ship is decided later.

- Definitions stay engine-neutral (only the extension list assumes Firefox).
- Settings → Apps → "Add a web app" builds one with the remote; advanced users can add any signed extension from a URL, with a warning.

### Firefox setup

Does: builds each web app's Firefox profile: kiosk mode, prefs, `policies.json`, copy protection, language, hardware decoding.

- Firefox is the web-app engine, probably for good; may be revisited once the work is deeper in. Claude flags it when web-app integrations become important.
- Firefox ships in the OS image, pinned and tested with our extension, `policies.json` and kiosk mode; latest release only; updates and rolls back with the OS ([Firefox in the OS image](decisions/0011-firefox-in-the-os-image.md)).
- Paid streaming: Firefox's copy protection is on and fetches Widevine on first use.
- Telemetry and studies off (see Local only).
- Each web app's Firefox language set from Hearth's setting through `RequestedLocales` ([how Firefox picks a language](research/L4-firefox-language.md)).
- Turn off pop-ups a remote can't close in every profile: password saving, notification requests, first-run pages.
- One Firefox profile per web app; uBlock Origin only where it's worth its RAM. Decided in the lighter-Firefox review (parked to near 1.0.0; starting point in [Firefox memory settings](research/F4-firefox-memory-prefs.md)).

Detail: [Firefox outside customising](research/2026-11-19-firefox-outside-customising.md), [Leanback's Firefox findings](research/2026-11-22-leanback-firefox-learnings.md), engines compared ([design doc detail, 8.3–8.6](research/2026-11-23-design-doc-detail.md)).

### `hearth-web-bridge`

Does: a small Mozilla-signed Firefox extension: Back on each site (close menu, go back, leave), page reports, `inputmode="none"`, a site's own menu items. Plus its native-messaging host, installed system-wide.

Decided:
- The lead and Claude develop it together, signed by Mozilla as unlisted.
- Named `hearth-web-bridge` (Claude's pick from the lead's two names, 2026-11-24: it carries Back and page reports as well as menu items). Research notes use its old name, `hearth-firefox-bridge`.
Waiting on the lead:
- Plain, unminified JavaScript with no build step; it passes Hearth's actions and page reports only, never page content or user data; force-installed through `policies.json` in the image.

Detail: [unlisted signing](research/M1-unlisted-extension-signing.md), [extension in the OS image](research/M2-extension-in-the-os-image.md).

## 6. OS image and releases

### Repo and code checks

Does: product code layout, build system, test framework, code checkers, code review, branch protection.

- Hosted code reviewers parked until we're ready to code; branch protection for `main` after them.
- Repo layout, build system, test framework, code checkers and five packages, as [proposed and tested](research/2026-11-24-product-code-setup.md) (the lead, 2026-11-24).

Waiting on the lead:
- Which hosted reviewers, if any, and which checks run locally ([agentic review checks](research/2026-11-19-agentic-review-checks.md)).

### Image build

Does: the mkosi configuration: pinned Fedora 45 snapshot, read-only `/usr`, writable `/etc`, `/var` and `/home`, every system language, the fonts, labwc, greetd, oo7, Firefox; AdGuard Home fetched and checked.

Decided:
- mkosi builds the image on Fedora 45 ([design doc detail, 5.1](research/2026-11-23-design-doc-detail.md) has the reasons).
- Read-only system: yes.
- AdGuard Home: pulled from AdGuard's own releases and pinned. Version and checksum pinned in the repo; the build fails on a mismatch. ([AdGuard Home import](research/2026-11-22-adguard-home-import.md)).
- The image blocks drivers for hardware an HTPC never has (FireWire, ISDN, old filesystems).
- Each Hearth release keeps the source archive of the AdGuard Home version it ships (GPL-3.0).

### Boot and updates

Does: systemd-boot with boot counting, sysupdate's transfer files, the good-boot check, update timing, the rollback message.

- systemd-sysupdate updates the image: A/B, the new version installs beside the old; automatic rollback if it fails to boot.
- "Healthy" means the home screen works: a Hearth unit before `boot-complete.target` holds the good mark until the shell reports in ([boot assessment and BIOS](research/2026-11-19-boot-assessment-and-bios.md)).
- Never restart for an update during viewing.
- "Viewing" means an idle inhibitor is held or an audio stream is running and not paused; recheck every 30 minutes; notification after 3 hours.
- Pin the systemd version per release and test the update path in CI. systemd 262 marks sysupdate experimental again and removes its D-Bus interface in 263 ([systemd 262](research/2026-11-24-systemd-262-installer-update-apis.md)).
- Update timers catch up after a wake.
- Tested in a VM: update, rollback of a broken version, and the good mark waiting for the shell. The boot file must be `Mode=0644`, or rollback never happens ([test image results](research/2026-11-24-test-image-results.md)).
- Hearth's own settings live in `/usr`, which each version replaces; `/etc` is copied from the image once, on first boot, and belongs to the box (the lead took Claude's pick, 2026-11-24).

### Installer

Does: the "Installer" boot profile running `systemd-sysinstall`, and the Hearth page that drives it.

- No flasher of our own. The box installs from an "Installer" boot profile running `systemd-sysinstall`, driven by a Hearth page ([Installer](decisions/0005-installer-boot-profile-sysinstall.md)).
- The default profile is made in the install flow.
- systemd 262, in Fedora 45, gives sysinstall an API with a drive list and progress; the Hearth page can use it, with a polkit rule for the installer page only ([systemd 262](research/2026-11-24-systemd-262-installer-update-apis.md)). Claude's proposal.

### Recovery and diagnostics

Does: the recovery menu (after three failed boots, or a held button at power-on): previous version, safe mode, reset settings, factory reset, diagnostics. The diagnostics bundle: cleaned-up logs to USB.

Decided: recovery mode, yes.

### Network and permission rules

Waiting on the lead (what listens, Claude's proposal): The firewall that keeps home-network services on the home network, polkit rules for privileged actions, mDNS on, home network only; SSH off, keys only; the DNS filter on this box only (sharing it with the home network is opt-in); Bluetooth discoverable only while the pairing page is open.

### CI

Does: builds the image on every change and for every Firefox security release; boots it headless in a VM and runs the shell's checks.

Decided: automated image builds are required for 1.0.0 (from Firefox in the image).

### Publishing

Does: release files and signing.

Decided: a raw disk image in a `.xz` plus a signed checksum; signing (GPG key and GitHub attestation) starts after alpha testing ([Release files](decisions/0006-release-files-raw-image-signed-checksum.md)). Hosting: any static host (GitHub Releases or the user's own mirror).

## 7. Profiles

Parental controls and profiles are a primary consideration.

### Login screen

Does: the profile picker, run by greetd as its own `greeter` user before anyone logs in; PIN entry.

Decided: boot shows the profile picker unless there is exactly one profile and it has no PIN ([Boot shows the profile picker](decisions/0004-boot-shows-profile-picker.md)).

### Profiles service

Does: creates and deletes profiles (Unix users), the groups that grant or take away permissions, checks PINs, the admin PIN, sets up each keyring, ends one profile's session when another logs in.

Decided:
- One Unix user per profile; one active session at a time, and a new login ends the old session ([One Unix user per profile](decisions/0002-one-unix-user-per-profile.md)).
- PINs: off by default. Users manage their own; accounts with parental controls enabled can't. Admins can set any profile's PIN to a new one. Once parental controls are on, every account needs a PIN to enter, except a kid profile, which needs the admin PIN to leave. Hearth checks every PIN itself ([Profile PINs](decisions/0018-profile-pins-self-managed.md)).
- The admin PIN is one box-wide PIN for the family's parents, asked for once parental controls are first used. It can guard settings, adding apps, and creating and deleting profiles, each a switch, all on by default. A forgotten admin PIN means a factory reset.
- Permissions come from Unix groups: a profile outside a group that grants a permission doesn't have it, and groups can also take permissions away (2026-11-22).
- Secret store: oo7, daemon and Flatpak portal; KWallet isn't used ([Secret store: oo7](decisions/0001-keyring-oo7.md)). Each profile's keyring unlocks from a random password sealed with `systemd-creds`, made at profile creation.
- When a keyring can't be opened: a fresh, empty keyring and one notice to sign in to apps again; no "Saved logins" page in 1.0.0 (2026-11-22).
- Plain `useradd` for 1.0.0, not systemd-homed (an encrypted home needs the user's password, which clashes with the no-prompt boot).
- Checked in the dev box (2026-11-24): Fedora 45's greetd login config includes `system-auth`, which loads `pam_systemd`, so its sessions should register with logind; `KillUserProcesses=` is off, so a logout alone can leave programs running and Hearth has to end the user with `loginctl terminate-user`.
- Tested in a VM ([test image results](research/2026-11-24-test-image-results.md)): greetd sessions register with logind; `loginctl terminate-user` ended Firefox, a Flatpak and a detached program in 0.6 seconds; `useradd` and `userdel` work with `/usr` read-only; the oo7 keyring opens with no prompt. Before deleting a user, Hearth ends every program it runs; Hearth checks for itself that the keyring opened.

### Guest profile

Agreed (the lead, 2026-11-25); needs more thought later:
- A Unix profile the admin can turn on or off, with its own PIN to give to guests.
- A guest can open apps, change accessibility settings and join a network, but can't forget saved networks or see saved Wi-Fi passwords.
- Parental controls apply; the guest can be set as managed or unmanaged.
- Leaving asks "Clear saved logins?" Yes deletes the guest's whole home folder and makes a fresh empty one (web-app logins live in Firefox profiles and app folders, not only the keyring). No keeps everything. If nobody answers (another profile logs in, sleep, power cut), the logins are cleared.

Claude's note: with the guest turned on there are always two profiles, so boot always shows the profile picker ([Boot shows the profile picker](decisions/0004-boot-shows-profile-picker.md)).

### DNS filter

Does: AdGuard Home as the box's ad and tracker blocker, with each profile as its own client; safe search and restricted mode for kid profiles; Hearth's settings drive it.

- AdGuard Home (one program, about 60–80 MB of RAM, safe search and restricted mode built in); its own web page off or bound to the box.
- Per-profile clients through the firewall (nftables), by Unix user. Works, Flatpak apps included ([test image results](research/2026-11-24-test-image-results.md)).
- How it runs: its own system user with only the port-53 right and `--no-check-update`. Works. Without systemd-resolved: its lookups skip a profile's rules (tested).

### Screen-time keeper

Does: counts each kid profile's time, warns ahead (user selectable time), ends the session; the admin PIN extends it.

### Parental-control screens

Does: allowed apps, screen time and bedtime, website limits (Firefox's policy, per profile), what the admin PIN guards, the activity summary (on the box only, opened with the admin PIN), PIN screens.

## 8. Hardware

### Hardware check

Does: a first-boot step, checked again for changes on every later boot ([Optional hardware](decisions/0008-optional-hardware-supported-never-assumed.md)), that finds the hardware tier, optional hardware present, video decode (by playing a sample), the NVIDIA generation, firmware, display mode, and whether suspend and wake work.

- hardware flags set automatically where possible (hardware detection, agreed). Test codecs by playing them; show codec status in Settings → About (from bifrost).
- Hardware tiers: supported (the floor and newer) and legacy (anything below that still boots), with a "Legacy hardware: fewer security guarantees" notice and a support end date in Settings → About.
- H.264 is set up on every box so it's available (the lead, 2026-11-24). Not forced: each site picks what it plays.

Detail: [video-decode review](research/2026-11-19-hardware-video-decode-review.md).

### Hardware add-ons

Does: codecs and outside drivers (NVIDIA, Broadcom Wi-Fi) built or fetched on the box for the hardware found, rebuilt after updates, falling back with a notice.

- anything only some boxes need is fetched during first-boot setup for the hardware found, not shipped in every image, so Hearth never redistributes it. A failed add-on never blocks an update.
- Each add-on is a system extension built on the box from RPM Fusion (codecs) or NVIDIA, tied to Hearth's image version; the old slot's extension stays for rollback.
- Setup automatically accepts NVIDIA's licence then the driver is fetched (confirmed by the lead, 2026-11-24).

Waiting on the lead:
- Where the compiler comes from: in every image, or fetched only on boxes that need it.

Detail: [NVIDIA on image-based Fedora](research/D1-nvidia-on-image-fedora.md), [drivers as system extensions](research/D2-driver-sysext.md), [NVIDIA support and licence](research/D4-nvidia-support-and-licence.md), codecs ([design doc detail, 5.3–5.3a](research/2026-11-23-design-doc-detail.md)).

### Gamepads and HDMI-CEC

Does: the input program for devices that don't arrive as keys: gamepad buttons (SDL3) and CEC remotes (libcec) turned into Hearth actions; pass-through mode for games and game streaming.

Decided: one comes later for gamepads and HDMI-CEC; no separate input program for keyboards and remotes (2026-11-22).

### Power and sleep

Decided: real suspend by default, tested on first run; screen-off if the test fails.

## 9. Settings and setup

### Settings app

Does: the settings screens, all reachable from the remote. The frame comes with the shell; each page ships with the feature it controls. Pages for hardware the box doesn't have are hidden or say what's missing. Built on system interfaces and KDE libraries that don't need Plasma.

| Page | What's on it | Built on |
|---|---|---|
| Network | Ethernet, Wi-Fi if present, password entry, box name | NetworkManager |
| Bluetooth | Only on boxes with Bluetooth: remotes, controllers, headphones | BlueZ |
| Display | Resolution, refresh rate, screen fit, TV colour range, HDR where possible | labwc's output settings; colour range needs a helper of our own |
| Sound | Outputs with the TV's own name; volume limit, mono, balance | PipeWire / WirePlumber |
| Remotes | "Press the button you want for Back", reset to defaults, key-repeat speed | Key handling |
| Apps | Installed apps, add a web app | Flatpak, web-app definitions |
| Profiles and parental controls | See Profiles | Profiles service |
| Appearance | Presets and sliders with live preview, day/night, fonts, rows and columns, background, menu side, theme packs from USB | Theme tokens |
| Accessibility | The 1.0.0 set | Several |
| Privacy and network activity | Every kind of connection, named, with its switch | Network rules |
| Updates | Status, pause or delay, mirror, roll back | systemd-sysupdate; not its D-Bus interface, which goes away in systemd 263 |
| Time and language | Time zone, region, language | timedated, localed |
| Power | Sleep timer, wake on remote, the power-cut setting explained | logind |
| Developer mode | SSH, more logs | — |
| About and help | Version, hardware tier, codec status, error history | — |
| Reset | Reset settings, or full factory reset | — |

- Overscan is handled inside Hearth, not by the TV or the kernel.
- Sound goes to the TV first and stays there when the screen sleeps (from bifrost).
- HDR is supported in 1.0.0: everything it needs is on every box, since storage and image size aren't major concerns (the lead, 2026-11-24). UI white at 203 nits in HDR.
- The WirePlumber rule that puts the TV's detected video output (HDMI/DisplayPort) first.

Waiting on the lead:
- TV colour range: Auto / Full / Limited ([TV colour range](research/A5-tv-colour-range.md)); how to set it under labwc is the spike's real-hardware check.
 
### First-boot wizard

Does: network, TV fit and colour range, the default profile, building add-ons, offering apps.

- first-boot wizard, yes. 

### Apps

Does: install, update and remove Flatpaks from Flathub; launch them full screen.

- Flatpak is fully supported for every app that isn't Hearth's own.

## 10. Docs and help

- Help pages: one per error code grouping, on the GitHub wiki.
- USB install guide: four flasher apps, written at 1.0.0 ([USB install guide](decisions/0007-usb-install-guide-apps.md)). It covers switching the firmware to UEFI, setting the power-loss option (AC Recovery) to Power On, and a firmware password.
- User and contributor docs kept apart, organised with Diátaxis (tutorials, how-tos, reference, explanation).

## 11. Phone

All phone features come in 2.0.0 (the lead, 2026-11-24): typing and voice typing, private listening ([Private listening](decisions/0010-private-listening-webrtc-then-bluetooth.md)), photos and files, casting a link, screen mirroring. What they're built on is decided then; earlier research is in [docs/research](research/).

## 12. After 1.0.0

Decided: casting (AirPlay, DLNA), screen mirroring, Tested with Hearth ([process](tested-with-hearth.md)) (2026-11-22). Also parked: app adapters for native apps' Back, a public SDK for the Menu Contribution API, Secure Boot and kernel lockdown, heavy accessibility (2.0.0), other languages.

Ideas, not scheduled: a Hearth API for web apps; native browse screens from service APIs; TV control over the home network for TVs without HDMI-CEC ([design doc detail, 6.5 and 8.6](research/2026-11-23-design-doc-detail.md)).
