# Roadmap to 1.0 (proposed)

*Claude's proposal, 2026-11-22, at the lead's request: "a roadmap to 1.0, features built, apis made, scripts completed, screens and UI stuff". Nothing here is decided. Built from the [design doc](design.md); where a row rests on a proposal rather than an agreed line, it says so.*

## How to read it

- **A milestone** ends with something you can try on a box. Milestones run in order; work inside one can overlap.
- **Each row is one piece of work**, which later becomes one GitHub issue under that milestone.
- **Kind** says what the piece is:
  - **Screen:** something you see and drive with the remote.
  - **Service:** a program running in the background, with no screen of its own.
  - **API:** an agreed way for two parts to talk (a message format, a D-Bus interface, a file format).
  - **Script:** a build, release or test tool.
  - **Config:** settings files that ship in the image.
  - **Test** and **Docs.**
- **Size** is Claude's rough guess at relative effort: S (small), M (medium), L (large). Not a time estimate.
- Every piece of work also gets its failure screens before it's done (design doc, section 15).

## The parts of Hearth, in one picture

| Part | What it is | Runs as |
|---|---|---|
| The OS image | Fedora 45 built by mkosi: read-only system, A/B updates, Hearth's parts inside | — |
| System services | Profiles and PINs, updates, add-on builder, DNS filter (AdGuard Home), hardware check | System (root or their own users) |
| The login screen | The profile picker, started by greetd before anyone is logged in | Its own `greeter` user |
| The session | labwc, the Hearth shell, the on-screen keyboard, notifications, portals | The logged-in profile's Unix user |
| Web apps | Firefox in kiosk mode, `hearth-firefox-bridge`, web-app definitions | The profile's user |
| Apps | Flatpaks from Flathub (Moonlight and Moonfin included) | The profile's user |
| Build and release | Image builds, signing, publishing, CI tests | GitHub Actions or the lead's machine |
| Docs and help | Install guide, user and contributor docs, help pages on the GitHub wiki | — |

## Milestone 0: Groundwork (now; no product code)

You can try: nothing new on screen. At the end, every open question is settled and every risky unknown has been tried.

| Work | Kind | What it is | Size |
|---|---|---|---|
| Test image in a VM | Spike | A mkosi-built Fedora 45 image boots on UEFI in a virtual machine and starts labwc with the spike shell | M |
| Updates and rollback | Spike | systemd-sysupdate installs a new version beside the old; a broken version rolls back after three tries; the "good boot" mark waits for the shell | M |
| Installer | Spike | The "Installer" boot profile runs systemd-sysinstall onto a blank virtual disk | S |
| Profiles and logins | Spike | greetd starts sessions logind knows about; ending a profile stops Firefox and a Flatpak app; adding and removing users with `/usr` read-only; a profile's keyring opens from a sealed password through oo7; Hearth checking PINs itself | M |
| Per-profile DNS | Spike | Each Unix user's DNS lookups reach AdGuard Home as a separate client, through the firewall | S |
| AdGuard Home as a service | Spike | Runs as its own user with only the port-53 right, beside systemd-resolved, with its updater off | S |
| Record a real boot | Spike | Every connection the test image makes, turned into the first allowlist file | S |
| Codec add-on | Spike | Build a codec system extension from RPM Fusion on the VM; rebuild it after an update | M |
| Keyboard beyond Firefox | Spike | The on-screen keyboard types into Flatpak apps (GTK4, Qt) and Moonlight | S |
| Portals in a Hearth session | Spike | Flatpak apps reach saved logins (oo7) and the file chooser with no desktop running | S |
| Firefox from outside | Spike | Which `policies.json` path Fedora's Firefox reads; a force-installed signed extension with no prompt; the system-wide native-messaging manifest; language, copy protection and studies settings | S |
| Product code layout | Docs | Claude proposes the repo layout, build system, test framework and code checkers for product code, for the lead's OK | S |

**Done when:** every spike above has passed or has a tested fallback, and the lead says product code can start.

## Milestone 1: A box that boots and updates itself

You can try: install Hearth from a USB stick in a VM and on the OptiPlex; it boots into a bare home screen; publish a new version and watch it update; publish a broken one and watch it roll back.

| Work | Kind | What it is | Size |
|---|---|---|---|
| Code review and repo guards | Script | The code-review setup the lead picks once we're ready to code, then branch protection for `main` (the lead's order) | S |
| Image build | Script | The mkosi configuration: pinned Fedora 45 package snapshot, read-only `/usr`, writable `/etc`, `/var` and `/home`, every system language, the fonts, labwc, greetd, oo7, Firefox | L |
| AdGuard Home fetch | Script | Downloads AdGuard's release, checks its checksum and signature, and puts it in the image; the build fails on any mismatch | S |
| Boot and update config | Config | systemd-boot with boot counting; sysupdate's transfer files; where updates come from | M |
| Good-boot check | Service | Marks a boot good only when the shell answers | S |
| Automated builds | Script | Builds the image on every change and for every Firefox security release | M |
| Publishing | Script | Release files: the raw image in `.xz` plus a checksum list (signing starts after alpha) | S |
| Session setup | Config | labwc settings from the spike: key bindings, full-screen rule, `hearth-send`, session start | S |
| Installer page | Screen | The Hearth page the Installer boot profile shows: pick a disk, confirm, install | M |
| VM test in CI | Test | Boots the built image headless in a VM and runs the shell spike's checks against it | M |
| Hardware checks on the OptiPlex | Test | labwc on a real screen, the overlay over playing video, TV colour range (spike checks 8, 10, 11) | S |
| Build and release how-to | Docs | Contributor docs for building and publishing an image | S |

**Done when:** a USB-installed box updates itself and rolls back a broken version, in a VM and on the OptiPlex.

## Milestone 2: The 10-foot loop

You can try: pick a tile, watch YouTube, press Back, Home and Menu, search with the on-screen keyboard, come home.

| Work | Kind | What it is | Size |
|---|---|---|---|
| Theme tokens | API | Every colour, size, font and timing in one place; the default dark theme; Atkinson Hyperlegible Next and Noto Sans; the TV-safe margins | M |
| TV controls | Screen | Hearth's own controls: tile, button, list, grid, slider, toggle, text box, dialog; the focus ring; focus never follows the pointer; an accessible name on every control | L |
| Translations | API | Every string through KDE's `i18n()` (KI18n), English only for now | S |
| App list and launching | Service | Installed apps (desktop files, Flatpaks, web apps), one app at a time, going home and back, forcing full screen | M |
| Home screen | Screen | Rows of tiles with the next row peeking in, focus and animation, background | M |
| Status overlay | Screen | Clock, network state, profile name, notification bell, power; burn-in shift | M |
| Key handling | Service | Back, Esc, Backspace, Home and Menu from every device, by what's in front; held keys | S |
| System menu | Screen | The overlay Menu opens: a system section always, an app section when the app contributes | M |
| Menu Contribution API, version 1 | API | How apps add menu items: the format, the limits, a D-Bus interface for native apps, native messaging for web apps; play and pause for any media app through MPRIS | L |
| Volume overlay | Screen | Volume and mute over any app, through PipeWire | S |
| On-screen keyboard | Screen | The real keyboard: QWERTY grid first, then alphabet grid and single row; layouts per kind of field; word suggestions (kept per profile, with an off switch); a real keyboard's letters pass through; held arrows repeat; key names through libxkbcommon | L |
| Failure screens | Screen | One failure-screen design, the list of human-readable error codes, a QR code to each code's wiki page | M |
| Web-app definitions | API | The file format for a web app (name, icon, address, extensions, which profiles may use it); YouTube TV, YouTube Music, Twitch and Dropout as built-ins | S |
| Firefox profiles | Config | Builds each web app's Firefox profile: kiosk mode, prefs, `policies.json`, copy protection on, telemetry and studies off, language from Hearth's setting, hardware decoding | M |
| `hearth-firefox-bridge`, version 1 | Service | The Firefox extension: Back on each site (close menu, go back, leave), page reports, telling the shell about `inputmode="none"`, a site's own menu items into the system menu | M |
| Bridge signing | Script | Signs the bridge as unlisted with Mozilla (`web-ext sign`); needs the lead's addons.mozilla.org account | S |
| Bridge host | Service | The native-messaging host the bridge talks to, installed system-wide | S |
| Test suite in CI | Test | The spike's headless tests grown into the product's, plus QML and C++ checkers | M |

**Flag for the lead:** this milestone is where web-app integration starts to matter. The lead asked to revisit Firefox against Qt WebEngine at that point (design doc, section 8.6).

**Done when:** YouTube, Twitch and Dropout work end to end with the air mouse on the OptiPlex.

## Milestone 3: First boot and profiles

You can try: start a new box, follow the setup with a remote, make a family of profiles with a kid profile and restrictions, switch between them.

| Work | Kind | What it is | Size |
|---|---|---|---|
| Hardware check | Service | Finds the hardware tier, optional hardware (Wi-Fi, Bluetooth, infrared, gamepad, CEC), video decode by playing a sample, the NVIDIA generation, whether suspend works | M |
| Add-on builder | Service | Builds codecs from RPM Fusion and outside drivers (NVIDIA, Broadcom Wi-Fi) as system extensions tied to the image version; rebuilds after updates; falls back with a notice; shows NVIDIA's licence to accept first | L |
| First-boot wizard | Screen | Network, TV fit and colour range, the default profile, building add-ons, offering Moonlight and Moonfin | L |
| Profiles service | Service | Creates and deletes profiles (Unix users), Unix groups that grant or take away permissions, checks PINs, the family's admin PIN (resets any PIN), sets up each keyring, ends one profile's session when another logs in | L |
| Permission rules | Config | polkit rules so only Hearth's services, and only for the right groups, can do privileged things | S |
| Profile picker | Screen | The login screen: pick a profile, enter its PIN if it has one; straight in when there's one profile with no PIN | M |
| PIN screens | Screen | Enter, set, change, remove, hint; the admin PIN's setup the first time parental controls are used ("a forgotten admin PIN means a full factory reset") | M |
| Profile switcher | Screen | Switch profile from the status overlay or the menu | S |
| Saved logins | Service | oo7 for each profile; if a keyring can't be opened, a fresh one and a single notice | S |
| DNS filter | Service | AdGuard Home with per-profile clients; safe search and YouTube restricted mode for kid profiles; Hearth's settings drive it | M |
| Parental controls | Screen | Allowed apps, screen time and bedtime, website limits (Firefox policy), what the admin PIN guards (settings, adding apps, profiles), the activity summary | L |
| Screen-time keeper | Service | Counts each kid profile's time, warns 5 minutes ahead, ends the session; the admin PIN extends it | M |

**Done when:** a fresh box set up from the sofa gives a family their own profiles, and a kid profile stays inside its limits.

## Milestone 4: Settings from the remote

You can try: change everything on the box from the sofa, with no keyboard.

| Work | Kind | What it is | Size |
|---|---|---|---|
| Settings app frame | Screen | The settings screens' layout, navigation and search | M |
| Network | Screen | Ethernet, Wi-Fi list and password (on-screen keyboard), box name; NetworkManager | M |
| Bluetooth | Screen | Only when the box has it: pair remotes, controllers, headphones; BlueZ | M |
| Display | Screen | Resolution, refresh rate, screen fit (inside Hearth), TV colour range, HDR where possible. labwc's display protocol has no colour-range setting, so colour range needs a small helper of our own (to find out how) | L |
| Sound | Screen + Config | Output list with the TV's own name; the WirePlumber rule that puts the TV first; volume limit, mono, balance | M |
| Remotes | Screen | "Press the button you want for Back", reset to defaults, key-repeat speed; default maps for infrared receivers | M |
| Gamepads and HDMI-CEC | Service | The input program for devices that don't arrive as keys: gamepad buttons (SDL3) and CEC remotes (libcec) turned into Hearth actions | M |
| Apps | Screen | Installed apps; install, update and remove Flatpaks from Flathub; add a web app | L |
| Appearance | Screen | Theme presets and sliders with a live preview, day/night, font choice, rows and columns, background; theme packs from USB (data only) | L |
| Accessibility (1.0 set) | Screen | Text size, high contrast, reduce motion, captions, mono audio, slower key repeat, ignore double presses | M |
| Privacy and network activity | Screen | Every kind of connection, named, with its switch; the Wi-Fi sign-in check | M |
| Updates | Screen | Status, pause or delay, mirror, roll back; through sysupdate's D-Bus interface | M |
| Time and language | Screen | Time zone, region, language (English only in 1.0) | S |
| Power | Screen | Sleep timer, wake on remote, the power-cut setting explained | S |
| Developer mode | Screen | SSH on or off, more logs | S |
| About and help | Screen | Version, hardware tier, codec status, error history with QR codes | S |
| Reset | Screen | Reset settings, or full factory reset | S |

**Done when:** every page in design doc section 10 works with only a remote.

## Milestone 5: Living-room ready (alpha)

You can try: use it as the household's TV box for a week without reaching for a keyboard.

| Work | Kind | What it is | Size |
|---|---|---|---|
| Notifications | Service + Screen | Hearth's notification service (apps freeze without one), the bell, short pop-ups | M |
| Update timing | Service | Downloads in the background, applies on a restart at an idle time, never while something is playing | M |
| Sleep, wake, screensaver | Service | Real suspend with wake from the remote; screensaver after idle; burn-in protection | M |
| Recovery menu | Screen | After three failed boots or a held button at power-on: previous version, safe mode, reset settings, factory reset, diagnostics | M |
| Diagnostics bundle | Service | Cleaned-up logs saved to USB or sent to the phone, by choice | S |
| Help pages | Docs | A wiki page for every error code | M |
| Network rules | Config | Each Hearth service may reach only its listed addresses; home-network-only services stay on the home network | M |
| Connection test in CI | Test | Boots the image, runs a scripted session, fails the build on any connection not on the allowlist | M |
| RAM on a 4 GB box | Test | Spike check 9 against the budget, then fixes | M |
| Home-screen extras | Screen | Arrange apps (Menu → Move), rows-and-columns setting, celebrations and particles on the home screen only | M |
| Alpha testers | Docs | A short tester guide and a way to report problems | S |

**Done when:** a week in a real living room with no keyboard, no SSH and no lost viewing.

## Milestone 6: 1.0

You can try: hand it to someone else to install and live with.

| Work | Kind | What it is | Size |
|---|---|---|---|
| Lighter Firefox | Test + Config | Measure on a 4 GB box from the research's starting point, cut what's worth cutting; uBlock Origin where it earns its RAM | M |
| Release signing | Script | GPG-signed checksums and GitHub attestation (after alpha, as decided) | S |
| USB install guide | Docs | The illustrated guide for the four flasher apps, with switching the firmware to UEFI | M |
| User and contributor docs | Docs | Tutorials, how-tos, reference and explanations, kept apart | L |
| Security review | Test | What listens, what connects, the threat model, the extension and the privileged services | M |
| Accessibility pass | Test | Every screen checked with the high-contrast theme and for accessible names | S |
| Beta | — | Wider testing; fixes only | — |

**Done when:** a stranger installs it from the guide and uses it as their TV box.

## Not placed yet (needs the lead)

- **The phone** (typing, then private listening, photos, casting a link): typing from a phone doesn't work on labwc with KDE Connect as it stands (design doc, section 7). In 1.0, or after?
- **Moonlight and Moonfin:** offered in the first-boot wizard (Claude's proposal), or installed on every box?
- **HDR output in 1.0**, or after (it needs labwc's Vulkan renderer)?
- **The word-suggestion engine** for the keyboard: a technical pick Claude can make in milestone 2 unless the lead wants to.

## After 1.0 (already parked)

Casting (AirPlay, DLNA), screen mirroring, Tested with Hearth, app adapters for native apps' Back, the Hearth API for web apps, TV control over the home network, Secure Boot and kernel lockdown, heavy accessibility (2.0), other languages.
