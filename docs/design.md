# Hearth design

*Started 2026-11-18. **Agreed** marks what the lead has said yes to; everything else is a proposal. Answered questions are deleted, not marked done. When an Agreed line is locked in, it moves to a decision record with why it was decided and what was turned down.*

## 1. Goals (the lead's)

1. Easy to install and keep running. Like a basic fridge: it works, and updates happen without the user noticing.
2. Replaces a smart TV. Phone pairing (as a remote and as a connected device), TV remotes as first-class input with sensible defaults, every app usable with a remote or controller, web apps as first-class apps, all settings reachable from the remote.
3. Local only. No telemetry, no collecting user data. Users can configure their box however they like.
4. Secure. AI makes attacks faster and cheaper, so security is a priority from day one.
5. Well documented. Easy-to-follow code, easier docs.
6. A plan for thorough agentic review that fits a solo developer working with AI (to be researched).
7. Anything the co-designer spots: sections 9–15.

## 2. Agreed

| Topic | Agreed |
|---|---|
| Starting point | Hearth writes its own shell in Qt 6 QML, with no plasmashell and no desktop session underneath. It borrows parts from Plasma Bigscreen with credit: the input handler, the link redirector, design ideas ([Borrow from Bigscreen](decisions/0012-borrow-from-bigscreen-write-shell-fresh.md)) |
| Licence | GPL-3.0-or-later (2026-11-22). Fits the borrowed Bigscreen input handler and the keyboard parts in section 6.4. GPL-2.0-only code, such as Bigscreen's link redirector, stays a separate program or is rewritten |
| Home screen and overlays | The lead's wants, listed in section 18: one full-screen app at a time; the installed apps in rows the user arranges, with the next row peeking in; a rows-and-columns setting; overlays; particle effects on the home screen only; a user-set background; a status overlay with burn-in protection |
| System menu | Hearth owns the menu the Menu button opens. Apps add their items through a Menu Contribution API, in 1.0. A public SDK for app makers comes after 1.0 (section 19) |
| Accessibility | Heavy features move to 2.0.0: screen reader, zoom, colour filters, dialogue boost, a separate simple layout. Cheap ones stay in 1.0 ([Heavy accessibility in 2.0](decisions/0013-heavy-accessibility-in-2-0.md)) |
| Compositor | labwc for 1.0 ([labwc for 1.0](decisions/0015-labwc-for-1-0.md); section 5.4) |
| Hardware floor | 4 GB RAM, x86-64, graphics with VP9 hardware decode: Intel 7th gen (Kaby Lake) or newer, AMD Raven Ridge-class APUs (2018) or newer, NVIDIA. Raspberry Pi 4 is a design goal only |
| Firmware | UEFI only. Boxes in legacy (BIOS) mode are switched to UEFI first; the install guide says how ([UEFI only](decisions/0014-uefi-only.md)) |
| Development | For now, in a Fedora 45 distrobox on the lead's machine ([dev environment](dev-environment.md)). Spike code lives on a local-only `spikes` branch that is never pushed (2026-11-22) |
| Optional hardware | On-board Wi-Fi, Bluetooth, USB infrared receivers, gamepads and HDMI-CEC are recommended and supported by default, never assumed present. Every core path (install, setup, recovery, settings, updates) works without them; features that need one appear only when it's there, or say what's missing ([Optional hardware](decisions/0008-optional-hardware-supported-never-assumed.md)) |
| OS image and updates | mkosi builds the image; systemd-sysupdate updates it (A/B: the new version installs beside the old, rollback if it fails to boot) |
| Read-only system, automatic rollback, recovery mode, first-boot wizard | Yes to all four |
| Installer | No flasher of our own: point to existing ones and write an illustrated guide (four apps, written at 1.0.0: [USB install guide](decisions/0007-usb-install-guide-apps.md)). The box installs from an "Installer" boot profile that runs `systemd-sysinstall`, driven by a Hearth page ([Installer](decisions/0005-installer-boot-profile-sysinstall.md)). Release: raw disk image in a `.xz` plus a signed checksum; signing (GPG key and GitHub attestation) starts after alpha testing ([Release files](decisions/0006-release-files-raw-image-signed-checksum.md)) |
| Base distro | Fedora 45 inside the mkosi image (section 5.1) |
| Remotes | USB air-mouse remotes, USB infrared receivers and Bluetooth remotes are first-class and all ship with default key maps; HDMI-CEC is a bonus. None is assumed present |
| Phone | KDE Connect as the base |
| Phone features, in priority order | 1 typing and voice typing · 2 private listening · 3 photos and files · 4 cast a link · 5 screen mirroring (revisit later) |
| Theming | High priority, built in from the start. Hearth-owned theme tokens (Kirigami route C), preview-first Appearance, day/night toggle (off by default, fixed times), no fonts in theme packs (open-source fonts ship in the image) ([Theming](decisions/0009-theming-presets-and-packs.md)) |
| Parental controls and profiles | Primary consideration |
| Borrowing | Borrow freely from open-source projects |
| Upstream etiquette | Follow each upstream project's AI policy; if it has none, assume they don't want our input (section 16) |
| No Chromium: scope | Our development doesn't build on or ship Chromium or Electron. Users may install Chromium-based apps |
| Notifications | Hearth ships its own notification service |
| KWallet | Not used |
| Profiles | One Unix user per profile; one active session at a time, and a new login ends the old session. Any profile, the default one included, can have a PIN: off by default, set and removed by the profile itself, recovered only by a hint where possible ([Profile PINs](decisions/0003-profile-pins-opt-in-self-managed.md)). Keyring set up when a profile is created. Default profile made in the install flow. Boot shows the profile picker unless there is exactly one profile and it has no PIN ([Boot shows the profile picker](decisions/0004-boot-shows-profile-picker.md)). Recorded in [One Unix user per profile](decisions/0002-one-unix-user-per-profile.md); research results in section 13.2 |
| Secret store | oo7, daemon and Flatpak portal ([Secret store: oo7](decisions/0001-keyring-oo7.md)). Each profile's keyring unlocks from a random password sealed with `systemd-creds`, made at profile creation ([Boot shows the profile picker](decisions/0004-boot-shows-profile-picker.md)) |
| On-box ad and tracker blocker | AdGuard Home. For now, Hearth pulls it from AdGuard's own releases and pins it (2026-11-22, section 13.1) |
| Dependency allowlist | Generate a reviewed list of the network calls our dependencies need (section 11) |
| Network defaults | AdGuard Home keeps its own defaults, upstream DNS included. Time source: the base image's default. Firefox's own connections allowed (Remote Settings on), telemetry off. Firefox studies: not decided. Firefox Safe Browsing off. Moonlight's launch-time connections accepted as they are. Captive-portal check ("Wi-Fi sign-in page") off, with a switch in Settings. Per-app audits are not the goal ([allowlist draft](research/2026-11-19-network-allowlist-first-draft.md)) |
| Kernel | Fedora's stock kernel, all CPU security mitigations on. Lockdown default: open (section 5.6) |
| Secure Boot | Off by default in 1.0 |
| Outside drivers | Drivers Fedora doesn't ship (NVIDIA's own driver, some Wi-Fi chips) are pulled at setup time for the hardware found, not built into every image (section 5.3a) |
| Sleep | Real suspend by default, tested on first run; screen-off if the test fails |
| Overscan | Handled inside Hearth, not by the TV or the kernel |
| Hardware detection | A first-boot step sets hardware flags automatically where possible: hardware tier, optional hardware present (Wi-Fi, Bluetooth, infrared, gamepad, CEC), firmware, display mode |
| Flatpak | Hearth's own parts are never Flatpaks; they live in the OS image. Flatpak is fully supported for every other app |
| Firefox (web-app engine) | In the OS image, not a Flatpak. Hearth pins the version and tests it with our extension, `policies.json` and kiosk mode before shipping; it updates and rolls back with the OS. Latest Firefox release only. Cost: every Firefox security release needs a new Hearth image, so automated image builds are required for 1.0 |
| The repo | Uses GitHub's free features: milestones, labels, Actions, Projects, security and code-quality tools, bots |
| On-screen keyboard: reach | Types into every app, including Flatpaks the user installs, not only Hearth's screens and web apps |
| On-screen keyboard: design | Designed from scratch once the compositor is chosen. Supports the common layouts: QWERTY grid (Android TV), alphabet grid (Roku), single row (Apple TV), gamepad wheel (Steam's old Daisywheel). The keyboard is themeable (section 9) |
| Languages | English only until 1.0.0. Support for other languages is scoped early so 1.0 doesn't block it |

## 3. Open

- On-screen keyboard details, including word suggestions. The compositor is chosen (labwc), so this is open now; the shell spike's test keyboard shows what labwc allows (section 6.4).
- Bundled apps, image or Flatpak: later (the lead, 2026-11-22). Firefox goes in the image. Are Moonlight and a Jellyfin client Hearth's own parts, or ordinary Flatpaks? The lead names Moonfin as the Jellyfin client to look at. Neither Moonlight nor Moonfin is in Fedora 45's own repos (package list; RPM Fusion not checked), so "in the image" would mean Hearth packages and patches them itself. Decides part of the codec plan (section 5.3).
- Kernel lockdown default: on, off, or on except on boxes that need an outside driver (section 5.6).
- NVIDIA floor: which generations count as supported, given NVIDIA's driver support (section 5.6).
- How a PIN guards a profile: later (section 13.2).
- Boot-time target: later.
- Casting in 1.0: AirPlay and DLNA receivers as opt-in switches, or later (section 7).
- Hosted code reviewers: deferred until the architecture is settled (section 17).
- Screen mirroring: revisit later.

## 4. Lessons from bifrost

bifrost, the lead's earlier personal setup, was read for lessons only; Hearth inherits none of its setup, apps or scripts. Requirements taken from it:

- Test codecs by playing them; a system can claim a format it can't play. Show codec status in Settings → About.
- Sound goes to the TV first and stays there when the screen sleeps.
- Ship a notification service. Without one, apps can freeze waiting for an answer.
- Ship a secret store that never prompts (where apps save logins). Without one, apps that save logins fail or keep them in plain files. KWallet isn't used. **Agreed:** oo7 ([decision record](decisions/0001-keyring-oo7.md)).
- Turn off pop-ups a remote can't close in every web-app profile: password saving, notification requests, first-run pages.
- Never restart for an update during viewing.
- Every app needs a known update path before it's offered.
- One global key map: a button does the same thing in every app.

Read again on 2026-11-22 for what bifrost verified about the test boxes and the TVs ([bifrost: test boxes and TVs](research/2026-11-22-bifrost-optiplex-and-tv.md)). More lessons from that pass (Claude's reading; each is a proposal where it changes the plan):

- Sound to the TV, the mechanism: the HDMI outputs share one sound card with the analog ones, and WirePlumber ranks analog higher, so Hearth needs its own priority rule. The TV's output exists only while the screen is on, so turning the screen off moves sound to the box's speaker. The box can read the TV's name from it (ELD), for Settings → Sound.
- Hearth's on-screen keyboard has to stay closed when a web app brings its own: on bifrost, Plasma's keyboard opened over Leanback for Dropout's (section 6.4).
- Some apps won't open full screen by themselves (Moonfin starts as a 1280×720 window), so Hearth forces it (spike check 12, section 5.4).
- "Something is playing" can be read from the system: an idle inhibitor is held, or an audio stream is running and not paused. Hearth's no-restart-during-viewing rule can use the same test (section 5.2).
- Bifrost turned suspend off because a sleeping box may miss its update timers and may not wake for the remote. Hearth's first-run suspend test (section 2) should include waking from each remote found, and update timers should catch up after a wake.
- Install guide: set the firmware's power-loss option (AC Recovery) to Power On, so the box comes back after a power cut.
- A TV without HDMI-CEC can still be controlled over the home network: bifrost did it for a Roku TV (section 6.5).

## 5. Platform

### 5.1 Base distro: Fedora 45 (Agreed)

mkosi builds from Fedora, Debian, Arch, openSUSE and others, so the base is a separate choice. Reasons for Fedora 45:

- It ships Qt 6.11 for the shell; Debian 13 has an older Qt.
- Its systemd is new enough for systemd-sysupdate's D-Bus interface (added in systemd 257), which Settings → Updates would use.
- `systemd-sysinstall`, the installer tool, needs systemd 261 or newer. Fedora 45 ships 261.2, Fedora 44 ships 259.8 ([Installer](decisions/0005-installer-boot-profile-sysinstall.md)).

Risk: systemd marks the sysupdate D-Bus interface experimental, so it can change between versions without notice. Pin the systemd version per release and test the update path in CI.

### 5.2 Updates

| What | How it updates | How often |
|---|---|---|
| The OS image (kernel, Hearth shell, services) | systemd-sysupdate, A/B, signed | Weekly, plus urgent security fixes |
| Apps the user installs | Flatpak from Flathub | Daily check |
| Firefox | In the image (section 2) | With the OS, plus an image for every Firefox security release |
| AdGuard Home | In the image, a pinned upstream release; its own updater off (section 13.1) | With the OS, plus an image for every AdGuard Home security release |
| Moonlight, Jellyfin client | In the image or Flatpak; open (section 3) | With the OS, or daily |
| Web-app definitions and their extensions | Hearth's own signed channel (section 8) | When the lead publishes; pinned versions |
| Codecs | Follows the OS (section 5.3) | With the OS |

- Rollback: systemd-boot boot counting. A new version gets three tries; if it never reaches "healthy", the box boots the old one and shows a message.
  - "Healthy" means the home screen works: Hearth reports a good boot only once the shell is up and responding. Otherwise a broken shell on a working system counts as a good boot and rollback never happens. Confirmed in systemd's docs: a Hearth unit ordered before `boot-complete.target` holds back the "good" mark until the shell reports in over D-Bus, the pattern systemd's own walkthrough uses ([boot assessment and BIOS](research/2026-11-19-boot-assessment-and-bios.md)).
  - Needs UEFI: systemd-boot doesn't run on BIOS-only machines. **Agreed:** Hearth boots on UEFI only ([UEFI only](decisions/0014-uefi-only.md)). Floor-era machines have UEFI firmware, and their legacy (BIOS) mode is a setting the user can switch off ([UEFI at the hardware floor](research/U1-uefi-at-the-floor.md)). The install guide covers switching it.
- Applying: download in the background, apply on a restart at an idle time, never during viewing. **Proposed** test for "viewing", from bifrost: an idle inhibitor is held (what video players take to keep the screen on), or an audio stream is running and not paused. Bifrost rechecked every 30 minutes and restarted anyway after 3 hours.
- Hosting: sysupdate reads plain files over HTTPS (an image plus a signed checksum list), so any static host works: GitHub Releases or the user's own mirror.
- User control: pause, delay, or point at a mirror (section 11).

### 5.3 Codecs

The lead asked whether codecs we can't ship can be layered in. They can. On a sysupdate image, layering means a system extension: a small add-on image laid over the read-only system. If Hearth builds and hosts a codec extension, though, Hearth is distributing the codecs, which is what we're avoiding.

**Proposed**, in order of preference:

1. Flatpak apps (anything the user installs, and the bundled apps if they end up as Flatpaks) get codecs from Flathub's runtimes, so Flathub does the distributing. From the [video-decode review](research/2026-11-19-hardware-video-decode-review.md): since the Freedesktop 25.08 runtime, codecs come in a `codecs-extra` extension that installs automatically; the old "install ffmpeg-full" advice is outdated. Whether Firefox gets hardware H.264 from it is unconfirmed. Plan: install `codecs-extra` and the Intel video-driver extension at build time, and add a build test that fails if H.264 isn't hardware-decoded on the reference Intel box.
2. Everything in the image (Hearth's own parts, and the bundled apps if they live there). Outside Flatpak, Fedora strips H.264 and HEVC from its Intel and AMD video drivers and from FFmpeg. This hits Firefox, which is in the image ([Firefox in the OS image](decisions/0011-firefox-in-the-os-image.md)). Tested in bifrost on Fedora 45 ([bifrost: test boxes and TVs](research/2026-11-22-bifrost-optiplex-and-tv.md)):
   - Fedora's Firefox lists H.264 as supported, then fails when the video starts: its FFmpeg hands H.264 to an empty stand-in.
   - Fedora's Intel driver decodes VP9 and AV1 on the GPU, but not H.264 or HEVC. RPM Fusion's `intel-media-driver` adds both and installs alongside Fedora's.
   - On AMD, RPM Fusion's `mesa-va-drivers-freeworld` installs alongside Fedora's `mesa-dri-drivers`.
   - Swapping in RPM Fusion's full FFmpeg replaces eight `-free` packages and nothing else.
   - With both in place, H.264 played and was hardware-decoded on one of the lead's boxes.

   The box builds its own codec extension on first boot from RPM Fusion and rebuilds it after each OS update, so Hearth distributes nothing. If the rebuild fails, the OS update still goes ahead and video falls back to CPU decoding with a notice. Codecs never block an update.

### 5.3a Hardware add-ons, pulled at setup time

**Agreed:** anything only some boxes need is fetched during first-boot setup for the hardware found, not shipped in every image, so Hearth never redistributes it. This covers codecs (item 2 above), NVIDIA's own driver, and Wi-Fi drivers Fedora doesn't ship (Broadcom `wl`, for example).

- How (**proposed**): each add-on is a system extension built or downloaded on the box for its exact kernel and OS version, and rebuilt after each OS update.
- A failed add-on never blocks an update. The box falls back with a notice: software video decoding, or the open-source NVIDIA driver.
- Each add-on source (RPM Fusion, NVIDIA) gets an entry on the connection allowlist (section 11).
- How others do it ([NVIDIA and Wi-Fi drivers on image-based Fedora](research/D1-nvidia-on-image-fedora.md), confirmed in their repos and docs):
  - Universal Blue and Bazzite build NVIDIA's kernel modules ahead of time, daily, in a separate image, and sign them in their own CI with their own key. Users enrol that key for Secure Boot. That is redistributing the driver, the route this section turns down.
  - RPM Fusion's `akmod` packages build the driver on the machine itself. After a kernel update, `akmods` rebuilds missing modules at boot; it needs a compiler and the kernel's build files, and the NVIDIA build can take up to 5 minutes. It signs what it builds with a key it makes on the box (`/etc/pki/akmods`).
  - Broadcom's `wl` Wi-Fi driver is in RPM Fusion's nonfree repo as `akmod-wl`, under a licence that reads "Redistributable, no modification permitted".
- What that means for Hearth (Claude's reading, not decided): the akmods route fits "built on the box", but on a read-only system the build can't write into `/usr`, and the compiler has to come from somewhere: in every image (bigger, more to attack) or fetched only at setup on boxes that need it. Packaging the result as a system extension is the open part: research prompt "Kernel drivers as system extensions" in the lead's log. A key made on the box ties into the lockdown question (section 5.6).

### 5.4 Compositor

**Decided (2026-11-22):** labwc is Hearth 1.0's compositor (the lead: "labwc for 1.0, now"), without waiting for the hardware checks. Record: [labwc for 1.0](decisions/0015-labwc-for-1-0.md). The comparison below is kept for the reasons.

gamescope is probably out: it needs Vulkan, which a lot of potato-class graphics lack or only half-support.

What Hearth needs from the compositor: one full-screen app at a time; Hearth's menu, volume and other overlays drawn on top of it; switching apps and returning home; and our on-screen keyboard typing into every app. Overlays drawn by a separate program use wlr-layer-shell, a protocol that lets a program draw panels and overlays above other windows. labwc and KWin support it ([compositor overlays](research/E1-compositor-overlays.md)); Weston and Qt Wayland Compositor don't ([Qt Wayland Compositor](research/E6-qt-wayland-compositor.md)).

| Candidate | For | Against |
|---|---|---|
| KWin on its own (no plasmashell) | Screen reader, zoom and colour-blindness filter built in (2.0 features now); HDR | Heavier; pulls in Plasma libraries ([compositor footprint](research/E4-compositor-footprint.md)) |
| labwc (small wlroots compositor) | Light; overlays, app switching and keyboard protocols all through standard protocols ([compositor overlays](research/E1-compositor-overlays.md), [compositor input protocols](research/E2-compositor-input-protocols.md)) | No screen reader or colour filters; refuses AI-made contributions and bug reports |
| Hearth as its own compositor (Qt Wayland Compositor) | Every app's window, Firefox included, becomes an item in Hearth's QML scene: animate it, scale it into a tile, layer overlays, with no change to the app. One process instead of two | Hearth takes on the whole compositor job: display output and modes, HDR, input devices, keyboard input into apps, screen-reader support. Protocol support unconfirmed |

cage is out (checked in its source, 2026-11-19): one full-screen app at a time, no layer-shell for overlays, and no input-method or text-input support, so the on-screen keyboard could only send fake key presses.

Weston with its kiosk-shell is out (checked in Weston's source, main branch, 2026-11-22):
- It speaks only text-input v1 and input-method v1 ([protocol list](https://gitlab.freedesktop.org/wayland/weston/-/blob/main/protocol/meson.build), [text-backend.c](https://gitlab.freedesktop.org/wayland/weston/-/blob/main/frontend/text-backend.c)). Firefox, Moonlight (SDL3) and GTK4 apps take on-screen-keyboard text only through text-input v3 ([apps and text input](research/K2-apps-text-input.md)). That's the same gap that ruled out cage. Adding v3 to Weston has sat as an open merge request for years ([MR 781](https://gitlab.freedesktop.org/wayland/weston/-/merge_requests/781), secondary).
- kiosk-shell makes every window full screen and has only background, normal and hidden layers: no overlay a system menu could use ([kiosk-shell.c](https://gitlab.freedesktop.org/wayland/weston/-/blob/main/kiosk-shell/kiosk-shell.c)). In Weston a "shell" is a C plugin inside the compositor, not a QML app like Bigscreen's "shell", so a custom Weston shell means writing compositor code in C.
- For it: light, packaged in Fedora 45 (15.0.1), supports HDR colour management.

With heavy accessibility moved to 2.0.0 ([Heavy accessibility in 2.0](decisions/0013-heavy-accessibility-in-2-0.md)), KWin's screen-reader, zoom and colour-filter lead matters for 2.0, not 1.0. labwc does everything 1.0 needs through standard protocols ([compositor overlays](research/E1-compositor-overlays.md), [compositor input protocols](research/E2-compositor-input-protocols.md)), so the shell could later move to another compositor that speaks the same protocols. Against labwc: no screen-reader support, and it refuses AI-made contributions, bug reports included ([compositor upkeep](research/E5-compositor-upkeep.md)), so Hearth takes it as it is.

Checked 2026-11-22: Fedora 45's labwc (0.20.2) offers every protocol on that list, plus display settings (output management) and screen power ([labwc protocols on Fedora 45](research/2026-11-22-labwc-protocols-fedora45.md)). labwc's HDR needs its Vulkan renderer ([compositor accessibility and HDR](research/E3-compositor-a11y-hdr.md)), the requirement that made gamescope unlikely. It only matters on HDR-capable boxes, but the spike should check that labwc's default renderer runs on floor graphics.

#### The labwc spike (agreed to run, 2026-11-22)

Spike code lives on the local-only `spikes` branch; results are written up in `docs/research/`. Checks and pass lines are Claude's proposal.

| # | Check | Passes when | Where |
|---|---|---|---|
| 1 | Bare home screen | A QML home screen fills labwc's screen; arrow keys move focus between three tiles | Dev box, labwc in a window |
| 2 | Firefox full screen | The home screen starts Firefox in kiosk mode; it fills the screen with no window frame | Dev box |
| 3 | Menu over Firefox | Menu opens a Hearth overlay (layer-shell, through layer-shell-qt) over full-screen Firefox; it takes the keys while open and gives them back when closed | Dev box |
| 4 | Typing | Stevia, a ready-made keyboard speaking input-method v2, types letters, Backspace and Enter into a Firefox text box | Dev box |
| 5 | Going home | Home brings the home screen to the front (foreign-toplevel); choosing Firefox again returns to it without restarting it | Dev box |
| 6 | Back key (section 6.1) | A remote's Back reaches the TV-navigation extension over native messaging in under 50 ms; Firefox never sees Esc and stays full screen | Dev box |
| 7 | Phone typing (section 7) | KDE Connect's remote input from the lead's phone lands in a Firefox text box, with no Plasma running | Dev box, needs the lead's phone |
| 8 | Real screen | Checks 1–5 again, labwc drawing straight to the screen with no desktop underneath; its default renderer works | OptiPlex, then the 4 GB box |
| 9 | RAM | Measured idle, and with one YouTube web app playing, against the budget in section 5.5 | 4 GB box |
| 10 | Overlay over video | The volume overlay over playing video drops no frames | OptiPlex, then the 4 GB box |
| 11 | TV colour range | Record whether full/limited range can be set under labwc, and how ([TV colour range](research/A5-tv-colour-range.md)) | A real TV |
| 12 | Forcing full screen | An app that opens as a window (Moonfin starts at 1280×720) is made full screen by Hearth, through a labwc window rule or the shell | Dev box |

**Agreed (2026-11-22):** real-hardware checks (8–11) come after the software checks are done (the lead: "real hardware is after software is done"). Phone typing (check 7) is for later.

Check 4, first try (2026-11-22, Claude): Stevia can't run on labwc. Both Fedora's 0.57.0 and upstream's main branch (2026-11-21) wait for `zphoc_device_state_v1`, a protocol only Phosh's compositor (phoc) offers, before they show any keyboard (`pos_wayland_has_wl_protcols()` in `src/pos-wayland.c`) [C, read in the source]. labwc doesn't offer it, so Stevia starts and never appears. Also found: the host desktop's IBus settings (`GTK_IM_MODULE=ibus`) leak into the dev box and make Firefox hand text boxes to IBus instead of labwc; the spike's session now drops them.

#### A working shell first (agreed 2026-11-22)

The lead: a working shell before setting up any hardware. It's spike code on the local `spikes` branch (the lead's choice over starting the real shell), built and run in the dev box with labwc in a window on the lead's desktop. Approved dependencies: Qt 6.11, layer-shell-qt 6.8, labwc 0.20, Firefox 160, and `wlr-protocols-devel` (added 2026-11-22), all Fedora 45 packages.

Version 0, as Claude proposed and the lead agreed:
- In: a home screen with the installed apps plus YouTube, Twitch and Dropout as Firefox web apps, in rows with the next row peeking in; arrow keys, Enter, Back and Home from a keyboard or the air mouse; apps open full screen and Home comes back; the Menu key opens Hearth's overlay over the running app ("go home", "close app"); a clock and a placeholder profile name; theme tokens from the first line, one dark theme, text sizes from the research.
- Out: settings pages, profiles and the picker, remote mapping, the on-screen keyboard, notifications, the phone, particles, the OS image.
- It covers spike checks 1–3, 5 and 12 above.

Built 2026-11-22; a headless test passed all 19 of its checks. Claude made the home screen an overlay (a layer-shell surface above every app) rather than a desktop the apps sit on, because only the overlay worked with the approved parts: going back to a running app on a desktop needs the window-switching protocol (foreign-toplevel), and its definition file wasn't installed. Fedora 45 packages it (`wlr-protocols-devel`, MIT licence). **Agreed (2026-11-22):** the home screen is a desktop the apps sit on, not an overlay (the lead: "rebuild as a desktop"). `wlr-protocols-devel` is approved as a dependency for the window-switching protocol.

### 5.5 RAM budget on a 4 GB box

Estimates; measure on real hardware.

| Part | Rough RAM |
|---|---|
| Kernel, systemd, services, DNS blocker | 300–400 MB |
| Compositor | 80–150 MB |
| Hearth shell | 150–250 MB |
| One Firefox web app (YouTube is the worst) | 600 MB–1.2 GB |
| Jellyfin playback | 150–300 MB |

Rule: one web app open at a time; going home closes it after a short grace period. zram (compressed swap in RAM) on.

### 5.6 Old hardware and security

- **Agreed:** Fedora's stock kernel, all CPU mitigations on, Secure Boot off by default in 1.0, outside drivers pulled at setup time (section 5.3a). Ubuntu-style "HWE kernels" aren't needed: Fedora already ships the newest stable kernel.
- Kernel lockdown: open. Claude's understanding, not verified: lockdown requires every kernel driver to be signed by a key the kernel trusts. A driver built on the box (NVIDIA's, Broadcom `wl`) isn't, so lockdown would block it, and with Secure Boot off there's no easy way to make the kernel trust a key the box makes itself. Options:
  - Off by default.
  - On, and switched off by the first-boot hardware check only on boxes that need an outside driver, with a notice in Settings → About. (**Proposed**)
  - On, with signed driver add-ons built by Hearth. That means redistributing them, which section 5.3a avoids.

  Lockdown also disables hibernation; sleep to RAM still works.
- Video decode by generation ([video-decode review](research/2026-11-19-hardware-video-decode-review.md)). The floor is where VP9 hardware decode starts:
  - Intel: 7th gen (Kaby Lake) adds VP9 and 10-bit HEVC and is the floor. 6th gen (Skylake) has neither, so it's legacy tier. 11th gen adds AV1, only with the non-free build of Intel's driver.
  - AMD: Raven Ridge APUs (2018) are the first APUs with VP9 decode and are the floor. The cut-off for discrete AMD cards isn't confirmed.
  - NVIDIA (supported):
    - VP9 decode from Pascal (GTX 10-series) on.
    - Needs NVIDIA's own driver, the `nvidia-vaapi-driver` bridge, and a Firefox setting that overrides its block on NVIDIA hardware decoding.
    - Not re-checked: NVIDIA's 590 driver series reportedly dropped Maxwell, Pascal and Volta. If so, Pascal cards stay on an older driver and the NVIDIA floor may be Turing (GTX 16 / RTX 20).
    - Hardware decode in the Firefox Flatpak on NVIDIA is unconfirmed.
  - Forcing H.264: an extension has to hide VP9 and AV1 from all three ways a page asks what the browser can play, running in the page's own script world. The common h264ify extension covers two. Needed only on legacy-tier boxes; switched on only when the first-boot hardware check finds no VP9 decode.
- Old drivers as an attack route (the lead's concern: AI finding holes in them). **Proposed:** one image, hardware tiers, fewer drivers loaded.
  - Supported tier: the floor and newer.
  - Legacy tier: anything below the floor that still boots (6th-gen Intel, for example). Best effort; Settings → About shows a "Legacy hardware: fewer security guarantees" notice and a support end date.
  - Fewer drivers: the image blocks drivers for hardware an HTPC never has (FireWire, ISDN, rare network protocols, old filesystems), as hardened distros do.
  - One image with tiers, instead of one image per hardware age, keeps building and testing to one image.

### 5.7 Flatpak: for apps, not for Hearth itself

- Hearth's own parts live in the OS image. Any other app can be a Flatpak from Flathub, and the shell supports Flatpak apps fully (install, update, launch, return home).
- Hosting our own Flatpak repo is cheap (static files plus a signing key), but each app we host is one more thing to keep updated. Not for 1.0.
- Instead: a curated "Tested with Hearth" list of apps and web apps tested with a remote, each with a rating and an app adapter (section 6.1). It's the shell's "app store" view. Process: [Tested with Hearth](tested-with-hearth.md).

### 5.8 Test boxes

From what bifrost recorded ([bifrost: test boxes and TVs](research/2026-11-22-bifrost-optiplex-and-tv.md)):

| | OptiPlex | ThinkCentre |
|---|---|---|
| Model | Dell OptiPlex 3080 Micro | Lenovo ThinkCentre M910q Tiny |
| CPU, graphics | Not recorded: a 10th-gen Intel (from memory), Celeron to i5; UHD 630 or UHD 610. Check in firmware setup (F2) | Core i5-7500T, HD Graphics 630: 7th gen, the floor generation |
| RAM | Not recorded | 8 GB |
| Video out | HDMI 1.4: 4K at 30 Hz at most, no HDR. 4K at 60 Hz only over DisplayPort | Not recorded |
| Wi-Fi | None fitted (empty M.2 slot), so likely no Bluetooth either | Wi-Fi |
| TV, remote | LG TV | TCL Roku TV (49S405); WeChip W1 air mouse |

- The OptiPlex is a ready test of "optional hardware never assumed": no Wi-Fi card, so likely no Bluetooth.
- The ThinkCentre already sits at the hardware floor in CPU and graphics; only its RAM is above it.

## 6. Remotes and input

**Agreed:** air-mouse, infrared and Bluetooth remotes are first-class and all ship with default key maps; CEC is a bonus. None is assumed present: setup and recovery work with whatever input is plugged in (section 2). With default maps most remotes need no setup; the "press each button" learning wizard is optional.

**Agreed:** one global key map. The user maps buttons to Hearth actions (Back, Home, Select, Play/Pause…) once, and each button means the same thing in every app. Making every app obey that map is Hearth's job.

Confirmed from kernel docs (remote-input chat):

- The kernel decodes the common infrared remote protocols itself.
- NEC-protocol remotes can't tell a held button from a repeated press, so "hold to do X" isn't reliable with them.
- CEC over DisplayPort only works with specific active adapters.

From bifrost, on the lead's WeChip W1 air mouse ([bifrost: test boxes and TVs](research/2026-11-22-bifrost-optiplex-and-tv.md)): its power and mouse-mode buttons send nothing the box can see; power is infrared only. It can learn the TV remote's power code and switch the TV with no box involved. So a default key map can't count on every button reaching Hearth.

### 6.1 Back, Esc and Backspace

The lead keeps fighting Firefox over these keys in Leanback for Dropout. The same keypress means different things to Firefox (Esc leaves full screen), to the site (Esc closes a menu) and to the remote's maker (some remotes send Backspace or BrowserBack for Back).

**Proposed:** apps never see the raw key.

1. Hearth's input service turns every remote button into a Hearth action (Back, Home, Select, Up, …) before any app gets it.
2. For web apps, the action goes to the Hearth TV-navigation extension over Firefox's native messaging (a supported channel from a local program to an extension), not as a fake keypress. The extension decides: close the site's menu, go back a page, or leave the app. Firefox never sees Esc, so full screen isn't affected.
3. For native apps, an app adapter turns the global actions into whatever that app understands (one needs Esc to go back, another Backspace). Users never see or edit adapters. Adapters come from the app's Tested with Hearth entry.
4. Home always goes home; it's never passed to an app.

Spike: native messaging latency (it must feel instant), and whether Firefox swallows any key before an extension sees it.

**Result (2026-11-22, headless, [shell spike results](research/2026-11-22-shell-spike-results.md)):** it works. labwc caught the remote's Back before Firefox; the shell sent it over native messaging to a test extension, which closed the page's menu, went back a page, and left the app, in that order. Firefox stayed full screen and the pages never saw Back or Escape. Slowest round trip: 7–8 ms. The spike installs the extension as a temporary add-on, because Fedora's Firefox installs only Mozilla-signed extensions for good; the product ships it signed (section 8.2).

**Agreed (2026-11-22):** the extension becomes `hearth-firefox-bridge`, a small Mozilla-signed Firefox extension that the lead and Claude develop together (the lead: "We can codevelop a small signed firefox extension as a hearth-firefox-bridge"). Back in other apps (step 3): later. Found by the lead in a window: Back did nothing in Firefox opened from its own tile, which had no extension. The spike now gives that tile its own profile and the extension too.

Confirmed by the lead's testing for Leanback for Dropout ([Leanback's Firefox findings](research/2026-11-22-leanback-firefox-learnings.md)):
- A page can't stop Firefox's Back. Cancelling `BrowserBack` doesn't stop the history-back (tested with the Dev Probe). So Hearth has to take the remote's Back before Firefox sees it, as step 1 says.
- What an air mouse's buttons arrive as in Firefox: Back as `BrowserBack` (some remotes send Escape), Home as `BrowserHome`, Menu as `ContextMenu`, which opens Firefox's own context menu unless something stops it.
- Esc takes a page out of its full screen, so Leanback makes video fill the window with CSS instead. Hearth never sends Esc to Firefox (step 2).
- Game controllers reach Firefox only through the Gamepad API, not as key presses, so the input service turns them into actions itself (section 6.2).
- A drifting air-mouse pointer must never move the selection. **Proposed:** a rule for every Hearth screen: focus never follows the pointer.

### 6.2 Key capture compared with Bigscreen

What Bigscreen does (read from its `inputhandler` source):

- A background service reads game controllers (through SDL3) and TV remotes over HDMI-CEC (through libcec).
- It turns each button into a fake keyboard key and types it into the window in front, through the RemoteDesktop portal (the interface remote-desktop software uses, provided by KWin).
- Remotes that already act like a keyboard (air mice, infrared receivers) are left alone; apps get their raw keys.
- The mapping tables are hard-coded and disagree with each other: the CEC remote's Back becomes Esc, the controller's Back button becomes the Back key, and the controller's B button becomes Cancel plus Esc. Each app then decides what those keys mean, which causes the Firefox problem.
- An "auto-suppress" setting stops the translation when an app wants the controller directly (games).

What Hearth would do instead:

| Step | Bigscreen | Hearth |
|---|---|---|
| Catch | Controllers and CEC only; keyboard-like remotes pass straight through | Every remote-like device, grabbed exclusively, so apps never see its raw keys. Full keyboards still type normally, minus a few keys Hearth reserves (Home) |
| Translate | Button → fixed keyboard key | Button → Hearth action (Back, Home, Select, …) using the device's map plus the user's remaps |
| Deliver to the home screen | Fake keys | Actions sent straight to the shell |
| Deliver to web apps | Fake keys; Firefox and the site both react | Actions sent to the TV-navigation extension over native messaging; Firefox never sees Esc or Backspace |
| Deliver to native apps | Same fake keys for every app; each app reads them its own way | The global action goes through the app's adapter, so Back always means back, even if one app needs Esc for it and another Backspace |
| Games and Moonlight | Auto-suppress | Pass-through mode; Hearth keeps only Home (held) |
| Phone typing (KDE Connect) | Separate path | Routed through the same service, so the phone follows the same rules |

- Keep from Bigscreen: the SDL3 controller code, the libcec code, the device watcher, and its permission rules for the kernel's virtual-keyboard device (uinput).
- New: the action layer, app adapters, the native-messaging bridge, exclusive grabbing, the global remap screen.
- How fake keys get typed: under labwc (chosen), through the kernel's uinput; there's no RemoteDesktop portal for it (section 7).
- Security: this service can type anything into anything. It gets no network access, accepts input only from local devices and KDE Connect, and logs nothing that's typed.

**Agreed (2026-11-22), from Claude's proposal:** keys are handled by what they are, not by which device sent them; this replaces "every remote-like device, grabbed exclusively" above for keyboards and remotes. The lead: navigation cohesion is what matters, not the device; "our primary input mode is the onscreen keyboard", and keypresses from keyboards and remotes with keyboards on them must still reach text boxes ("your proposal is good").
- labwc's key bindings catch Back, Home, Menu, Esc and Backspace from every device, and the shell decides what each means now. Esc is Back. Backspace deletes when an app's text box has focus and is Back anywhere else.
- Arrows and Select drive the on-screen keyboard while it's shown and go to the app otherwise. Anything that types (letters, digits, space) goes into the text box, whichever device sent it.
- No separate input program for keyboards and remotes. One comes later for gamepads and HDMI-CEC, which don't arrive as keys; the rest of the table above is its plan.

Built and tested headless in the shell spike the same day ([shell spike results](research/2026-11-22-shell-spike-results.md)).

**Agreed (2026-11-22), held keys, from Claude's proposal:** Back, Esc, Home and Menu fire once, when the key is let go; only Backspace in a text box and arrows in the on-screen keyboard repeat (the lead: "agreed"). Why: labwc repeats a held bound key 25 times a second, so a held Back walked out of an app in the spike. Long-press actions (hold Home for a pass-through toggle, section 6.2) would need the press and release timed, which the on-release binding doesn't give; NEC infrared remotes can't hold anyway (section 6).

Found 2026-11-22 (Claude, from the lead's WeChip air mouse on the dev computer, read from `/sys/class/input`) [T]:
- The air mouse is one USB receiver (`248a:8367`, "Telink Wireless Receiver") showing up as four input devices: a mouse (2 buttons), "Consumer Control" (149 keys declared, including Back, Home, Menu, Select, arrows, volume, play/pause), "System Control" (power, sleep, wake) and a keyboard (146 keys declared: letters, space, Backspace, arrows, Compose).
- So a remote can't be told from a keyboard by the keys it declares. Hearth needs a list of known remotes by USB ID, or to ask at setup ("is this a remote?"). Claude's proposal, not decided: handle keys rather than whole devices. Remote keys (arrows, Select, Back, Home, Menu, media) become actions; letters and the rest pass through as typing, so a remote with a keyboard on its back still types.
- Reading input devices needs the `input` group or a permission rule; on Linux Mint the desktop already gives the logged-in user `/dev/uinput` (for creating virtual devices).

### 6.3 Remapping after setup

Settings → Remotes → pick the remote → a drawing of a generic remote with the button highlighted → "press the button you want for Back". One map for everything, no per-app overrides, and a "reset to defaults" option. Works with a remote, a phone or a keyboard.

### 6.4 On-screen keyboard: parts to borrow (proposed)

*Claude's proposal, 2026-11-22. Nothing here is decided. Checked in each project's own repo that day.*

No single keyboard does what Hearth's must: remote and gamepad first, several key arrangements, themeable, typing into every app. Several do one part each.

On labwc, our keyboard would use three protocols: input-method v2 (the compositor tells the keyboard a text box is focused, and the keyboard sends text back), text-input v3 (the same thing on the app's side), and virtual-keyboard v1 (for keys with no text: Enter, arrows, Backspace). labwc has all three ([compositor input protocols](research/E2-compositor-input-protocols.md)).

| Keyboard | What to borrow | Licence | Notes |
|---|---|---|---|
| [plasma-keyboard](https://github.com/KDE/plasma-keyboard) (KDE, Qt 6 QML; what Bigscreen uses) | Layouts as packages (dozens of languages), D-pad navigation, word candidates, text composers for complex scripts | GPL, mostly GPL-2.0-only OR GPL-3.0-only; some files Qt's GPL-3.0-only ([existing keyboards](research/K3-existing-keyboards.md)) | Speaks input-method v1, which only KWin offers. Its compositor link would be replaced with v2 for labwc |
| [Qt Virtual Keyboard](https://github.com/qt/qtvirtualkeyboard) | QML layouts and styles, arrow-key navigation, word-prediction plug-ins | GPL-3.0-only or commercial | Runs inside one Qt app, not system-wide, so it'd need wrapping. plasma-keyboard has solved a similar wrapping problem |
| [Stevia](https://gitlab.gnome.org/World/Phosh/stevia) (Phosh's keyboard, GTK4) | How to drive input-method v2, virtual-keyboard and layer-shell on a wlroots-based compositor, the same set labwc offers. Word completion (presage) and correction (hunspell) | GPL-3.0-or-later | Active: v0.59.0, 2026-11-14. Touch-first. Fedora 45's build (0.57.0) uses GTK3 and libhandy. Needs phoc's private `zphoc_device_state_v1`, so it doesn't run on labwc as it is (section 5.4) |
| [wvkbd](https://github.com/jjsullivan5196/wvkbd) | The smallest C example of the same protocols | GPL-3.0 | Touch and pointer only |
| [Squeekboard](https://gitlab.gnome.org/World/Phosh/squeekboard) | Same protocols plus text-input v3 | GPL-3.0 | Marked unmaintained. Reference only |
| [Kodi](https://github.com/xbmc/xbmc/tree/master/system/keyboardlayouts) | 31 remote-friendly keyboard layout files (XML), one per language | GPL (Kodi's) | Data, not code |
| [gamepad-osk](https://github.com/0x90shell/gamepad-osk) | Ideas for a controller-driven keyboard (Go, SDL3, layer-shell) | MIT | Small, new, looking for Wayland testers (secondary) |

- Gamepad wheel (Steam's old Daisywheel): no maintained open-source one found for Linux. Hearth designs its own.
- What the makers' own pages say about their layouts ([TV keyboard layouts](research/K4-tv-keyboard-layouts.md)): little. None of Google, Roku, Apple, Microsoft, Sony or Valve states its key arrangement in text. Confirmed details:
  - Apple TV shows a single-row keyboard with the Siri Remote and a grid with other devices. Play/Pause cycles lowercase, uppercase, numbers and symbols; holding a key gives accents.
  - Gboard on Google TV picks a layout per kind of field (text, email, password, number). text-input v3 tells our keyboard the same thing (its "content purpose"), so Hearth can do this too.
  - Gamepad shortcuts that recur: X for delete, Y for space (Windows' gamepad keyboard, Google's old ADT-1 kit).
  - Confirming the arrangements themselves needs screenshots or the devices. The four layouts agreed in section 2 come from the lead.
- **Agreed (2026-11-22):** spike check 4 uses a small test keyboard written in the spike, borrowing Stevia's two protocol files (input-method v2, virtual-keyboard v1), pinned to Stevia v0.57.0 with checksums (the lead: "A"). Stevia itself doesn't run on labwc (section 5.4).
- Check 4 result (2026-11-22; the lead confirmed in a window with the air mouse: "keyboard works"; details headless, [shell spike results](research/2026-11-22-shell-spike-results.md)): the test keyboard types into Firefox on labwc, driven by arrows and Select through input-method v2's keyboard grab, with Backspace and Enter through virtual-keyboard. Found: Firefox doesn't pass `inputmode="none"` on; Qt apps switch text input on with no text box focused; a real keyboard's keys were swallowed while the keyboard was shown, fixed since by handling keys by what they are (section 6.2).
- Proposed combination: a Qt QML keyboard that matches the shell and its theme, built from plasma-keyboard's layouts and navigation, talking input-method v2 the way Stevia does.
- Password fields: text-input v3 marks them (purpose `password`, hint `sensitive_data`), so the keyboard turns word suggestions off there ([Wayland keyboard protocols](research/K1-wayland-keyboard-protocols.md)).
- Web apps with their own keyboard: on bifrost, Plasma's on-screen keyboard opened over Leanback for Dropout's own one ([bifrost: test boxes and TVs](research/2026-11-22-bifrost-optiplex-and-tv.md)). Leanback's fix is the web's own: its text boxes are marked `inputmode="none"` (and `ime-mode: disabled`), meaning "no system keyboard here" ([Leanback's Firefox findings](research/2026-11-22-leanback-firefox-learnings.md)). **Proposed:** Hearth's keyboard honours that mark. Tested 2026-11-22: Firefox doesn't pass it on through text-input v3 (the keyboard opened anyway), so the mark would have to reach the shell through `hearth-firefox-bridge` instead (Claude's proposal).
- Licences: GPL-3.0-only and GPL-3.0-or-later code mix, and so do the "GPL-2.0-only OR GPL-3.0-only" files if we take them as GPL-3. GPL-2.0-only code doesn't mix with them.

### 6.5 TV control without HDMI-CEC (idea, not scheduled)

*Claude's proposal from bifrost's Roku bridge, 2026-11-22. Nothing here is decided.*

HDMI-CEC lets the remote turn the TV on and change its volume, but it's a bonus (section 6), and many PCs can't send it at all (from memory: Intel and AMD graphics have no CEC on HDMI). Bifrost found another route ([bifrost: test boxes and TVs](research/2026-11-22-bifrost-optiplex-and-tv.md)): talk to the TV over the home network.

- What bifrost did, tested on a TCL Roku TV (Roku OS 16.0.4): power, volume, mute and input switching through Roku's External Control Protocol. It found the TV on the network and woke it with Wake-on-LAN. The remote's volume keys went to the TV, not the box.
- What the TV needed: "Control by mobile apps → Network access: Enabled" (the default, "Limited", refuses key presses since Roku OS 14.1) and "Fast TV start: On" (so it can be woken from standby).
- For Hearth it would be a "home network only" connection (kind C, section 11): named, listed, and off until the user pairs a TV. Each TV brand has its own protocol; bifrost has nothing on LG's.
- How bifrost caught the remote's keys: it grabbed the remote's device exclusively and passed the other keys on through a virtual device. Hearth handles keys by what they are instead (section 6.2), so that's for gamepads and CEC only, if at all.

## 7. Phone

**Agreed:** KDE Connect as the base. Its mobile app is weak but works; improve it only if KDE's AI policy allows (section 16).

| Priority | Feature | How | Effort |
|---|---|---|---|
| 1 | Typing and voice typing | KDE Connect's remote input sends keystrokes; the phone keyboard's own voice typing works through it | Low |
| 2 | Private listening (TV sound in your headphones) | First a WebRTC page on the phone (no app, no radio on the box); lip sync by delaying Hearth's own player, with Firefox following its audio output's reported delay. Then Bluetooth headphones paired to the box, classic and LE Audio as one path, shown only when the box has a radio ([Private listening](decisions/0010-private-listening-webrtc-then-bluetooth.md)) | High; needs a spike |
| 3 | Photos and files | KDE Connect "share" sends a file; Hearth shows it in a "From your phone" viewer | Medium |
| 4 | Cast a link | KDE Connect sends a link; Hearth opens the matching web app (a YouTube link opens the YouTube app at that video). Bigscreen has a link redirector to start from | Medium |
| 5 | Screen mirroring | iPhone: AirPlay via UxPlay. Android: Linux Miracast receivers are immature | Revisit later |

Findings from the phone and casting chat (sources in the [design log](research/2026-11-18-design-log-remote-input-phone-casting.md)):

- KDE Connect needs ports 1714–1764 on the home network. Its iPhone app is partial.
  - Spike: does KDE Connect typing reach a Firefox web app under our compositor, without Plasma? Later (the lead, 2026-11-22).
  - Checked in the files, not tried (Claude, 2026-11-22): on Wayland, KDE Connect 26.08.3 types through the desktop's remote-control portal (RemoteDesktop) and libei; its typing plugin (`kdeconnect_mousepad.so`) links libei and calls `ConnectToEIS` [C]. Fedora 45's portal for wlroots compositors (`xdg-desktop-portal-wlr` 0.8.5) offers only Screenshot and ScreenCast [C, its `wlr.portal` file]. So under labwc, KDE Connect would pair but couldn't type into apps.
  - **Agreed (2026-11-22):** revisit later; the core experience comes first.
  - The lead's idea (2026-11-22, not decided): KDE Connect may need to be forked at some point and become a standalone Hearth feature or app.
  - That chat recommended a Hearth phone web page instead. KDE Connect stays the choice; the web page is the fallback if the spike fails.
- AirPlay: UxPlay (GPL-3) mirrors the screen and plays audio from iPhones, but not Apple-protected video. It needs an H.264 decoder, which ties into the codec issue (section 5.3).
- DLNA ("play to"): gmrender-resurrect is a headless receiver.
- Google Cast: a real receiver needs credentials issued by Google, and the open workarounds are fragile. Skip it.
- Every receiver is a listening port, so each gets a name and a switch (section 11).

Pairing: section 12.3.

## 8. Web apps

### 8.1 User-built web apps

A web-app definition is a small file: name, icon, URL, user agent (spoofed if a site only gives its TV interface to TVs), extensions, adapter (normally the TV-navigation extension), and which profiles may use it. Settings → Apps → "Add a web app" builds one with the remote. The built-in ones (YouTube, YouTube Music, Twitch, Dropout) use the same format.

YouTube's TV interface (`youtube.com/tv`) works in desktop Firefox; no user-agent spoofing is needed for it (the lead, from experience, 2026-11-22).

### 8.2 Extensions and their update path

- Release Firefox only installs extensions signed by Mozilla. That covers both cases the lead wants: extensions listed on addons.mozilla.org, and extensions in a GitHub repo that Mozilla signs as "unlisted" (Leanback for Dropout works this way).
- Each definition pins an extension version and its checksum. A new version reaches boxes only when a new definition is published on Hearth's web-app channel, never by auto-installing the latest release (section 12.4).
- Advanced users can add any signed extension from a URL, with a warning that it updates on its own schedule.

**hearth-firefox-bridge** (agreed 2026-11-22, section 6.1): Hearth's own small extension, signed by Mozilla as unlisted. What Mozilla's rules say ([unlisted signing](research/M1-unlisted-extension-signing.md), [extension in the OS image](research/M2-extension-in-the-os-image.md); research results, wording to check before a decision record):
- Signing: an addons.mozilla.org (AMO) developer account and API keys, then `web-ext sign --channel unlisted`. Automatic checks first, "up to 24 hours"; a person may review it later, at any time, and can block it. [C]
- Every add-on follows Mozilla's Add-on Policies, listed or not. Code must be reviewable; minified code is allowed, obfuscated code isn't, and code from bundlers or build tools needs its source attached. Data sent to a native program falls under the same consent rules, and leaking user information through native messaging is banned. Permissions must be only what it needs. [C]
- Mozilla has no policy on AI-written add-on code (NOT FOUND in the policies). Firefox's own AI coding policy covers patches to Firefox, not add-ons. [C] Signing our own extension is fine: the ground rule is for contributions to someone else's code (the lead, 2026-11-22; section 16).
- Getting it into every profile from the image: a `policies.json` with `ExtensionSettings` → `force_installed` and a `file:///` address can install a signed extension and stop users removing it. [C] Where Fedora's Firefox reads `policies.json` from isn't documented: `/etc/firefox/policies/` or the install's `distribution/` folder. The box has `/usr/lib64/firefox/distribution/` and `/etc/firefox/pref/`. Extensions dropped in the system-wide extensions folder need the user's approval, so that route doesn't suit an appliance. [C]
- The native messaging manifest goes in `/usr/lib64/mozilla/native-messaging-hosts/` (Mozilla's docs; the folder exists in Fedora's Firefox package). [C]
- **Proposed (Claude):** plain, unminified JavaScript with no build step, so no source upload; the bridge passes Hearth's actions and page reports only, never page content or user data; installed through `policies.json` in the image, with the manifest system-wide. To test in the dev box: which `policies.json` path Fedora's build reads, and whether a policy install skips every prompt (needs a signed extension to test with).

### 8.3 Browser engine: no Chromium

**Agreed:** Hearth's own development avoids Chromium, so no single company decides how the web works. That rules out Electron and Bigscreen's own web-app viewer (QtWebEngine is Chromium). Users can install whatever they like.

| Engine | Who's behind it | Fit for Hearth today |
|---|---|---|
| Gecko (Firefox) | Mozilla | Use for 1.0. Mature; runs YouTube, Twitch, Dropout; has extensions (uBlock Origin, Leanback for Dropout) |
| WebKit, WPE port | Apple leads WebKit; Igalia maintains WPE, which was built for set-top boxes | Lighter on RAM, but no Firefox-style extensions, so no uBlock Origin or Leanback for Dropout. Fallback only |
| Servo | Linux Foundation Europe | Embeddable, not ready for YouTube-class sites |
| Ladybird | Ladybird Browser Initiative (nonprofit) | Pre-alpha; first alpha aimed at 2026 for Linux and macOS |

**Proposed:** Firefox for 1.0, with web-app definitions kept engine-neutral (only the extension list assumes Firefox), so Ladybird or Servo can replace it when ready.

### 8.4 Netflix and other paid streaming: "may not work"

These need Widevine, Google's copy-protection module, which Firefox downloads from Google.

- Off by default, behind an opt-in switch: "Paid streaming (uses Google's copy protection)".
- Labelled "may not work". On Linux, quality is often capped well below a smart TV's.

### 8.5 Why Firefox and not our own browser

| Option | What it means | Verdict |
|---|---|---|
| Electron | Chromium plus Node.js, one copy per app | Out: it's Chromium. Each Electron app also ships its own Chromium, often weeks behind on security fixes, which we would have to own |
| Our own browser on Gecko | Desktop Gecko can't be embedded in another app any more (Mozilla's embedding kit is Android-only), so it means forking Firefox (as LibreWolf and Zen do) or keeping a third-party embedding layer alive (as Sailfish OS does) | Not for 1.0. Keeps extension support, but we'd own a browser's security releases. Research task in the lead's log |
| Our own browser on WPE WebKit | A small launcher around WPE (Cog is an existing one). Full control of keys and the window | Possible, but loses Firefox extensions (uBlock Origin, Leanback for Dropout) and some site compatibility |
| Firefox in kiosk mode plus our extension | Mozilla does the security work; our extension adds TV navigation; native messaging gives us the key control (section 6) | Use for 1.0 |

Revisit if one Firefox web app doesn't fit the 4 GB RAM budget. A WPE-based launcher is then the next candidate. (The other trigger is answered: the Back-key spike found no key Firefox takes before Hearth, section 6.1.)

### 8.6 Later: our own browser, and a Hearth API for web apps

*Exploration from November 2026, scoped as research in [the lead's log](questions-and-tasks.md).*

- Our own lightweight Gecko browser keeps Firefox extension support but brings security upkeep:
  - Forking Firefox (LibreWolf, Zen) means rebuilding on every Firefox release: every four weeks, sooner for security fixes.
  - Embedding Gecko in QML has one known example, Sailfish OS, whose small team has spent years catching up to Firefox versions Mozilla had already dropped.
  - Middle route: stock Firefox changed from outside with policies, prefs, its own style sheet and AutoConfig. Checked 2026-11-19: this goes far enough that a fork isn't needed. AutoConfig with its sandbox off gives a startup script full browser rights, so it can rebind the browser's keys and change its interface without a rebuild. It can break on Firefox updates, which the pinned, tested image catches ([Firefox outside customising](research/2026-11-19-firefox-outside-customising.md)). It can't make the page an item inside Hearth's Qt scene. Open question: Firefox, or Bigscreen's Qt WebEngine (Chromium), which can.
- A Hearth API for web apps: a small script our extension adds to every web app for consistent Back handling, theme colours, focus style and overlay requests. LG webOS ships a similar JavaScript library for its web apps. Works with the kiosk route.
- Native screens from service APIs: Hearth draws the browse screens and the page only plays video. Easy for Jellyfin, partial for Twitch, hard for YouTube (Google key, quotas, terms), not possible for Dropout.

### 8.7 What Leanback for Dropout learned about Firefox

From the lead's own extension ([Leanback's Firefox findings](research/2026-11-22-leanback-firefox-learnings.md)), for Hearth's web-app profiles and its TV-navigation extension:

- Autoplay with sound can only be allowed by the profile (`media.autoplay.default = 0`) or a policy; no extension can grant it. Hearth's web-app profiles already set it.
- Firefox claims H.264 support even when it can't decode it. Leanback checks by decoding one tiny frame: a concrete way to do "test codecs by playing them" (section 4).
- A page can't close its own window; the extension's background script can, and closing the last tab in kiosk mode ends Firefox. That's how "close the web app" and Back on a site's home can work.
- Set each kiosk profile's home page to its site, so a stray Home key that reaches Firefox does no harm.
- Hardware decoding in a kiosk profile: `media.ffmpeg.vaapi.enabled = true`.
- One kiosk profile takes about 0.5–1 GiB of RAM (not measured), in line with section 5.5.
- Remotes that send many quick presses instead of one held press: treat a press within 0.15 s of the same button's release as a hold; OK, Back and Menu never repeat. Input for the Hearth input service (section 6).

## 9. Theming

Theming is built in from the start, because adding it later means finding every hard-coded colour. The shell uses theme tokens from its first screen.

| Layer | What users change | Where | Ships in |
|---|---|---|---|
| 1. Tokens | Colours, fonts, text size, corner roundness, spacing, animation speed, focus highlight style, UI sounds | Settings → Appearance with live preview; presets plus simple sliders (colour pickers are hard to use with a remote) | 1.0 |
| 2. Theme packs | Tokens, wallpapers, icons and sounds in one folder; fonts are picked from the open-source fonts in the image, never shipped in a pack | Import from USB or from the phone; share online | 1.0 |
| 3. Layout skins | Rearrange the home screen (rows, grid, sidebar) | QML files, which are code, so developer mode only | Later |

- Theme packs are data only, never code, so a downloaded theme can't run anything.
- Accessibility themes (high contrast, large text) are ordinary themes. An automatic test checks every built-in theme against WCAG AA contrast rules.
- Web apps take the focus highlight and overlay colours from the theme; the sites themselves are unchanged.
- Kirigami (KDE's QML toolkit): [Theming](decisions/0009-theming-presets-and-packs.md) was written when the shell was a Bigscreen fork, so it planned a small bridge for leftover Kirigami controls. With a fresh shell ([Borrow from Bigscreen](decisions/0012-borrow-from-bigscreen-write-shell-fresh.md)), Hearth's own tokens drive everything and the bridge is needed only if Hearth uses Kirigami controls at all. Whether it does is open (lead's log).

### 9.1 What other platforms offer (research results)

From the makers' support pages for Apple TV, Google TV, LG, Samsung, Roku, Xbox, PlayStation 5 and Steam Big Picture ([TV appearance settings](research/A1-tv-appearance-settings.md), [console appearance settings](research/A2-console-appearance-settings.md)):

| Setting | Who offers it | Hearth |
|---|---|---|
| Light / dark / automatic | Apple (automatic by time of day); Xbox (light, dark, or scheduled by set times or sunrise and sunset). LG: no option | Day/night toggle at fixed times ([Theming](decisions/0009-theming-presets-and-packs.md)) |
| Text size, bold text | Apple, Google (slider), PlayStation, Steam (UI scaling), Samsung ("Zoom Menu and Text"). Roku says it has none | Text-size token, 1.0 |
| High contrast | Apple, Google (experimental), LG, Samsung (black background, white or yellow text), Xbox, PlayStation, Steam | High-contrast themes, 1.0 |
| Reduce motion | Apple, PlayStation, Steam | Animation-speed token, 1.0 |
| Reduce transparency | Apple; Samsung's high contrast makes menus opaque | Not planned yet |
| Focus style | Apple ("High Contrast" focus) | Focus-highlight token, 1.0 |
| Bigger home-screen tiles | Apple ("Large": 5 icons per row, not 6), PlayStation ("Large Layout") | Rows-and-columns setting (section 18) |
| Accent colour | Xbox (system colours, sliders; hex code coming) | Theme tokens; presets plus sliders |
| Backgrounds | Xbox (custom image, solid colour), PlayStation (images, slideshow), Roku | User-set background (section 18) |
| Theme packs | Roku: colour, background, screensaver and sounds in one pack | Theme packs, 1.0 |
| Colour filters, invert, greyscale | Apple, Samsung, LG, Xbox, PlayStation, SteamOS | 2.0.0 ([Heavy accessibility in 2.0](decisions/0013-heavy-accessibility-in-2-0.md)) |
| Zoom / magnifier | Samsung (up to 200%) | 2.0.0 |

### 9.2 Text size, fonts and HDR white (research results)

- **Smallest text** ([TV text size](research/A4-tv-text-size.md)): Google's older TV style page gives 12sp, which is 24 px on a 1080p screen (default body text 18sp, 36 px). BBC's TV guide gives 24 px body copy, on a canvas size it doesn't state. Apple gives 23 pt (default 29 pt); that 1 pt = 1 px at 1080p is from memory. **Proposed:** the theme's smallest text is 24 px at 1080p, body text around 32–36 px, checked from a sofa on a real TV.
- **Fonts** ([open-source fonts](research/B5-oss-fonts.md)): all the candidates are OFL-licensed except Intel's Clear Sans (Apache 2.0). Fedora 45 packages: Inter (`rsms-inter-fonts`), Noto Sans (`google-noto-sans-fonts`), Atkinson Hyperlegible Next (`atkinson-hyperlegible-next-fonts`), Source Sans 3 (`adobe-source-sans-pro-fonts`), Red Hat Text (`redhat-text-fonts`), IBM Plex Sans (`ibm-plex-sans-fonts`), Fira Sans (`mozilla-fira-sans-fonts`), Open Sans, Roboto, Public Sans, Clear Sans. Lexend has no Fedora font package. Clear Sans was archived in 2023 and Fira Sans's repo is archived; Public Sans says it isn't actively maintained. Only Atkinson Hyperlegible Next (made for readers with low vision) and Lexend make legibility claims, and neither cites independent tests. Which fonts ship and which is the default: open (lead's log).
- **UI white in HDR** ([UI white on HDR screens](research/A7-ui-white-on-hdr.md)): the broadcast standard (ITU-R BT.2408) puts graphics white at 203 nits; GNOME defaults to 203; KWin's code says 200, but new KWin setups start at 500. **Proposed:** 203 nits by default in HDR mode, adjustable. Low priority until HDR output is tested.

## 10. Settings from the remote

Bigscreen's settings pages are tied to Plasma. Rebuild them on system interfaces and KDE libraries that don't need Plasma.

Pages for optional hardware the box doesn't have (Wi-Fi, Bluetooth, CEC, gamepads, infrared) are hidden or say what's missing (section 2).

TV colour range ([TV colour range](research/A5-tv-colour-range.md)): if the PC sends full range (0–255) and the TV expects limited range (16–235), the darkest greys all show as black; the reverse turns black into dark grey. Intel and AMD drivers share one kernel setting, "Broadcast RGB" (Automatic, Full, Limited). Intel's Automatic picks limited for TV modes on HDMI; AMD's Automatic means full, so AMD boxes on TVs may crush blacks out of the box. NVIDIA's own driver has an X11-only setting; nothing found for Wayland. How Hearth sets it under labwc is spike check 11 (section 5.4).

| Page | What's on it | Built on |
|---|---|---|
| Network | Ethernet, Wi-Fi list if the box has Wi-Fi, password entry (phone typing helps), hostname | NetworkManager (NetworkManagerQt) |
| Bluetooth | Only on boxes with Bluetooth. Pair remotes, controllers and headphones; discoverable only while this page is open | BlueZ (BluezQt) |
| Display | Resolution, refresh rate, overscan / screen fit, HDR where possible, TV colour range (Auto / Full / Limited; proposed) | The compositor's output settings: on labwc, the output-management protocol, which has no colour-range setting (section 5.4) |
| Sound | Output (TV, headphones, analog), with the TV named as it reports itself (ELD); volume limits, dialogue boost, mono audio | PipeWire / WirePlumber, with a Hearth rule that puts the TV's HDMI output first (section 4) |
| Remotes | Remap (section 6.3), learning wizard, key-repeat speed | Hearth input service |
| Phone | Pair or unpair; what the phone is allowed to do | KDE Connect |
| Apps | Installed apps, "Tested with Hearth" list, add a web app | Flatpak and web-app definitions |
| Profiles and parental controls | Section 13 | Hearth profile service |
| Appearance | Section 9 | Theme tokens |
| Accessibility | Section 14 | Several |
| Privacy and network activity | Section 11 | Hearth and the firewall |
| Updates | Status, pause or delay, mirror, roll back to the previous version | systemd-sysupdate D-Bus |
| Time and language | Time zone, language, region | systemd timedated / localed |
| Power | Sleep timer, wake on remote, what happens after a power cut (a firmware setting, AC Recovery: the page explains it, since the OS can't set it on most boxes) | systemd-logind |
| Developer mode | SSH on/off, more logs, layout skins, custom extensions | — |
| About and help | Version, hardware tier, codec status, error history with QR codes | — |
| Reset | Reset settings, or full factory reset | — |

## 11. Local only, still tweakable

The lead's concern: a rule of "update checks are the only outgoing connection" conflicts with user tweaking, phone features, and AI agents adding network calls unnoticed. The rule instead:

> Hearth never makes a connection the user didn't ask for or switch on, and every kind of connection is named, listed and switchable in Settings → Privacy and network activity.

| Kind | Examples | Default | User can |
|---|---|---|---|
| A. Needed to work | Time sync (secure websites need a correct clock), DNS | On | Point at their own servers |
| B. Updates | OS image, Flatpak apps, web-app definitions | On | Pause, delay, point at a mirror, turn off (with a warning) |
| C. Home network only | Phone (KDE Connect), casting, mDNS name (`hearth.local`), SSH in developer mode | Phone on; SSH off | Turn each off; the firewall keeps these on the home network, with no internet access |
| D. What you open | YouTube, Twitch, Jellyfin, Moonlight | When used | Their choice |
| E. Opt-in extras | Paid-streaming copy protection, the network's "Wi-Fi sign-in page" check | Off | Turn on |
| Never | Telemetry, analytics, automatic crash uploads | — | — (crash reports leave through a QR code the user scans; section 15) |

Enforcement:

1. Per-service network rules. Each Hearth service runs under systemd rules listing which addresses it may reach; a service with no business on the internet can't reach it, whatever its code says.
2. An egress test in CI. CI boots the built image in a virtual machine, leaves it idle, runs a scripted session, and records every DNS lookup and connection. Anything not on a reviewed allowlist fails the build. This catches an AI agent, an upstream package or a new dependency that starts phoning home.
3. The allowlist is a file in the repo; only the lead approves changes to it.
4. The first allowlist is generated from our dependencies (**Agreed**; to do). List every network call each dependency might make: Firefox's defaults, Flatpak, systemd (time sync, sysupdate), NetworkManager's connectivity check, KDE Connect, AdGuard Home's filter-list downloads, fonts and certificate updates. Each entry: who calls, where to, why, default on or off, how to turn it off or repoint it. Method: read each project's docs and config, then confirm by recording a real boot. Tracked in [the lead's questions and tasks log](questions-and-tasks.md).

Tweaking: system files are read-only, but configuration lives in a writable layer that survives updates. Developer mode unlocks SSH, custom sources and layout skins.

## 12. Security

### 12.1 Updates are the main defence

- Signed images; the box refuses an unsigned or tampered update.
- Firefox is the biggest exposure. It lives in the image (section 2), so every Firefox security release triggers a Hearth image release, which needs automated image builds.
- Urgent security fixes can trigger an out-of-cycle image release.
- Secure Boot is off by default in 1.0 (**Agreed**). Kernel lockdown default: open (section 5.6). UEFI only (**Agreed**, section 5.2).
- AdGuard Home is fetched from AdGuard at image-build time, so its supply chain is ours to check (section 13.1).

### 12.2 What listens, and why

The lead's concern: attack surface, for us and for users. The approach: secure defaults, plus switches that say what they open.

| Thing that listens | Default | Reach | Why it's needed / how it's limited |
|---|---|---|---|
| KDE Connect (ports 1714–1764) | On after pairing | Home network only | Phone features. Refuse old-protocol devices (12.3) |
| mDNS (`hearth.local`, casting discovery) | On | Home network only | Finding the box by name |
| SSH | Off | Home network only | Developer mode; keys only, no passwords |
| DNS blocker | On | This box only | Sharing it with the whole home network is an opt-in switch |
| Bluetooth | On, if the box has it | In the room | Only discoverable while the pairing page is open |
| Firefox | When used | The internet | The biggest risk: untrusted web content. Fast updates, one profile per app, a sandbox |
| Physical access (USB) | — | In the room | Out of scope beyond Secure Boot and the read-only system |

### 12.3 Phone pairing

- Best practice is to confirm the connection out of band: the TV and the phone each show a short code (or the phone scans a QR code from the TV), and the user confirms they match. This defeats someone on the network posing as one side. Matter smart-home devices use a related idea: a setup code turned into a shared key.
- KDE Connect today: encrypted connections (TLS), and protocol version 8 shows a verification key on both devices during pairing. Three flaws in 2025, from KDE's own advisories ([KDE Connect 2025 security bugs](research/C1-kdeconnect-2025-bugs.md)):
  - The 8-character pairing code was short enough to brute-force a matching key and pose as a device (CVE-2025-32898). Fixed by protocol 8: desktop 25.04, Android 1.33.0, iOS 0.5.
  - A forged network broadcast could change another device's shown name or type (CVE-2025-32900). Same fix.
  - Protocol 8 itself never checked that the device ID matched across two handshake messages, so a paired device could be impersonated. Rated critical (CVE-2025-66270). Fixed in desktop 25.12, Android 1.34.4, iOS 0.5.4.
  - Fedora 45 ships the desktop app at 26.08 (26.08.3 in the dev box), past all three fixes. KDE published two more advisories on 30 Oct 2026 that haven't been read yet (prompt in the lead's log).
- QR-code pairing: no KDE Connect app supports it. The desktop, Android and iOS source has no QR code at all, and no feature request was found ([QR pairing and old versions](research/C2-kdeconnect-qr-and-versions.md); KDE's bug search was blocked to the research tools, so one may exist). It stays a candidate contribution, only if KDE's AI policy allows.
- Refusing older devices: Fedora 45's version (26.08) refuses only a paired device that drops to an older protocol than it used before; an unpaired old device only gets a log warning. KDE's 26.11 beta refuses every network device below protocol 8, with no switch (commit `94b1f0a`, 13 Sep 2026). So the rule arrives with the next KDE release Fedora ships (Fedora updates KDE apps within a release: from memory).
- **Proposed:**
  - Require protocol version 8: wait for KDE's release that drops older versions rather than patching it ourselves. Ship desktop 25.12 or later in any case.
  - Pairing only works while Settings → Phone → Pair is open; the window closes after two minutes.
  - Show the verification key large on the TV.
  - Unpair from the TV or the phone.

### 12.4 Supply chain

If a box automatically installs the newest release from a GitHub repo (an extension, say), and that account is taken over or a bad change slips into a release (an AI agent's mistake included), every box installs it within a week. Mozilla's signing doesn't stop this: unlisted signing is automatic, not human-reviewed.

Fix: pin versions and checksums for everything pulled from outside (extensions, app bundles downloaded from GitHub, Flatpaks we list). A new version reaches boxes only through a release the lead approves. Plus protected release branches on the lead's GitHub (two-factor login is on, 2026-11-22).

### 12.5 Web content

One Firefox profile per app (a bad page in one app can't see another app's logins), Firefox's own sandbox (plus Flatpak's, if Firefox ships as one), fast updates, uBlock Origin everywhere (malicious ads are a common attack route), and no Chromium (section 8.3).

## 13. Profiles and parental controls

A primary feature.

| Control | How |
|---|---|
| Profiles | Picture and name; optional PIN. One admin profile owns settings |
| Allowed apps | Per profile; kids can't add apps or open settings |
| Movie and TV ratings | Each Hearth profile maps to its own Jellyfin user, which has a maximum rating |
| YouTube restricted mode, SafeSearch | Forced through DNS (YouTube and Google support this officially) |
| Website limits in web apps | Firefox's website-filter policy, per profile |
| Screen time | Daily limit and bedtime hours; 5-minute warning; admin PIN to extend |
| Leaving a kid profile | Needs the admin PIN |
| Activity summary | Kept on the box only; visible to the admin profile |
| Limit | Someone with the box in hand can boot another system. The guide covers setting a BIOS password |

### 13.1 On-box ad and tracker blocker

**Agreed:** AdGuard Home rather than Pi-hole: one program instead of several, about 60–80 MB of RAM, SafeSearch and YouTube restricted mode built in. Open source (GPL-3.0), made by the company AdGuard. Its own web page is off or bound to the box itself; Hearth's settings drive it.

- Lighter alternative: Blocky (about 30–50 MB), but without SafeSearch support, which we'd have to build.
- Per-profile filtering: each profile is its own Unix user, so the firewall (nftables) can tell their DNS lookups apart and hand them to AdGuard Home as separate clients. A kid profile then gets stricter rules and forced safe search in every app, not only Firefox. Pointing each profile's Firefox at the blocker with its own client name is weaker: Firefox only accepts an `https` address for that, and it covers Firefox alone ([AdGuard per-profile check](research/2026-11-19-adguard-per-profile.md)). Spike: can nftables make each user's lookups arrive from a different local address, so AdGuard sees separate clients? Fallback: one small AdGuard Home per profile. AdGuard's login stays on and only Hearth's system service holds it, because a kid's session can reach the box-local API too.
- uBlock Origin is still partly needed. AdGuard Home blocks whole ad and tracker domains by DNS, for every app on the box. It can't block:
  - ads served from the same domain as the content; YouTube's ads come from the same servers as its videos.
  - leftover ad boxes, pop-ups and cookie banners on the page, which uBlock Origin hides inside the browser.

  Twitch puts ads into the video stream, which neither fully blocks. **Proposed:** AdGuard Home for the whole box; uBlock Origin only in web apps where it's worth its RAM (likely YouTube). Decided in the lighter-Firefox review (section 17).
- Sharing the blocker with the whole home network: opt-in only, because it makes the box a network service (section 12.2).

#### Getting AdGuard Home into the image

Facts from [AdGuard Home import](research/2026-11-22-adguard-home-import.md), checked 2026-11-22:
- Fedora 45 doesn't package it (neither AdGuard Home nor Blocky), so the image build fetches it from AdGuard.
- AdGuard's releases on GitHub carry a SHA-256 checksum list, and the program inside each archive is signed with AdGuard's OpenPGP key (`2864 5AC9 776E C4C0 0BCE 2AFC 0FE6 41E7 235E 2EC6`). Both checks pass for v1.0.0 (19 Nov 2026). An AdGuard page confirming that fingerprint wasn't found yet.
- It updates itself unless started with `--no-check-update`, which also removes its update check to `static.adtidy.org`.
- From 1.0, patch releases (1.0.x) are bug and security fixes; minor releases add features and may drop old API calls.

**Agreed (2026-11-22):** "we'll pull it ourselves and pin for now" (the lead). Claude reads this as option 1 below: AdGuard's own release, version and checksum pinned in the repo. Checking the signature as well was part of Claude's option 1; the lead didn't speak to it.

Options as Claude put them:

| | Option | For | Against |
|---|---|---|---|
| 1 | **AdGuard's own build, pinned and checked twice** (Claude's pick) | The repo pins the version, the archive's SHA-256 and AdGuard's key fingerprint. A fetch step before the image build downloads the archive, checks both, and fails the build on any mismatch; mkosi's `ExtraTrees=` copies the program into `/usr`. No compiler; the same file in every image of that version | Trusts AdGuard's build machine |
| 2 | Build it from source during the image build | We'd build what we read | Needs Go plus Node.js and npm, and the web interface pulls hundreds of npm packages that would each need pinning: a bigger supply chain for the same program |
| 3 | Our own RPM package around AdGuard's build | The package database knows the files | Same file as option 1 plus a spec file to maintain. Can follow later |

Turned down by Claude: third-party package repos (Copr, Snap), since someone else's build becomes our supply chain.

What's hard to undo: little. Moving from option 1 to 2 or 3 changes only the build step.

How it would run (proposed, not decided):
- The program is read-only in `/usr/bin`; its config and data live in `/var/lib/adguardhome`, which survives updates.
- Hearth's own systemd unit starts it with `--no-check-update`, `--work-dir` and `--config` pointing there, and `--web-addr` bound to the box itself. It runs as its own system user allowed only to use port 53 (from memory; spike).
- DNS listens on the box only. systemd-resolved either forwards to it or has its own listener switched off: spike.
- A new AdGuard release reaches boxes only in a Hearth image the lead approved (section 12.4). A security release triggers an image, as for Firefox (section 5.2).
- GPL-3.0: each Hearth release keeps the source archive of the AdGuard Home version it ships (from memory: GPL-3.0 requires offering the source).

### 13.2 How profiles run (research results)

Decided already: one Unix user per profile, one session at a time, optional PIN, a picker at boot unless there's one profile with no PIN ([One Unix user per profile](decisions/0002-one-unix-user-per-profile.md), [Profile PINs](decisions/0003-profile-pins-opt-in-self-managed.md), [Boot shows the profile picker](decisions/0004-boot-shows-profile-picker.md)).

- **Starting a session** ([greetd sessions](research/P1-greetd-sessions.md)): greetd is a small login service (in Fedora 45). Its greeter, the picker screen, runs as its own `greeter` user and hands greetd the chosen user name. greetd checks the password through PAM, Linux's login-check system, and starts the session as that user. `initial_session` logs one user in automatically once per boot, which fits the one-profile, no-PIN case. The new session gets the screen and input through logind. Unconfirmed: whether Fedora's greetd login config registers the session with logind (`pam_systemd`); check in a test image.
  - Claude's note: under greetd, the picker is a separate small program run before any profile's session, not part of the shell.
- **Ending a session** ([ending sessions](research/P2-ending-sessions.md)): `loginctl terminate-user` ends all of one user's sessions and processes. `KillUserProcesses=` kills a session's processes at logout; the upstream default is yes, Fedora's is unconfirmed (Claude recalls no). Each user's own copy of systemd stays 10 seconds after the last session ends, and apps it started may outlive the session. So "a new login ends the old session" needs `terminate-user`, and a test that Firefox and a Flatpak app really stop.
- **PINs** ([PIN as password](research/P3-pin-as-password.md)): Fedora has no PIN login, so a PIN would be a short ordinary password. Fedora's password-quality check refuses anything under 6 characters (default 8) when a user sets their own password; root can still set one, with a warning. If the failed-login lock is switched on, 3 wrong tries lock the profile for 10 minutes (man-page defaults; Fedora's own unconfirmed). How a PIN guards a profile is open and can wait (the lead, 2026-11-22). The options to bring then: Hearth checks the PIN itself, or a root service sets the PIN as the Unix password; the research advises against the second.
- **Creating and deleting profiles** ([creating users](research/P4-creating-users.md)): `useradd` and `userdel` write only to `/etc` and `/home`, so they should work with a read-only `/usr` (test on an image). `userdel` refuses while the user has processes running, so deleting a profile ends its session first. systemd-homed offers encrypted, portable home folders; its tool is in Fedora 45's `systemd-udev` package, but whether the service ships is unconfirmed. Claude's note: an encrypted home opens only with the user's password, which clashes with the no-prompt boot. **Proposed:** plain `useradd` for 1.0.
- **An admin profile** ([polkit rules](research/P5-polkit-admin.md)): polkit is the service that decides who may do privileged actions (Fedora 45 ships version 127). Its rules are small JavaScript files; Hearth's would sit in `/usr/share/polkit-1/rules.d`, inside the read-only image, so no profile can change them. A rule can let members of one Unix group do one action without a password, and Hearth's system services ask polkit over D-Bus (`CheckAuthorization`). So an admin profile can be built as a Unix group plus Hearth's rules. Whether one exists at all is the parental-controls question in the lead's log.

## 14. Accessibility

**Agreed:** heavy features move to 2.0.0, split as in the Release column ([Heavy accessibility in 2.0](decisions/0013-heavy-accessibility-in-2-0.md)).

| Need | What Hearth does | Built on | Release |
|---|---|---|---|
| Blind and low-vision users | Screen reader for every Hearth screen, switchable from the first boot screen (hold Back for 5 seconds, as many TVs do) | Qt accessibility → AT-SPI → Orca | 2.0.0 |
| Low vision | Text size, high-contrast themes | Theme tokens | 1.0 |
| Low vision | Zoom | Compositor zoom | 2.0.0 |
| Colour blindness | Never colour as the only signal | Design rule | 1.0 |
| Colour blindness | Colour filters | Compositor filter | 2.0.0 |
| Deaf and hard of hearing | Captions on by default if chosen once; caption size and colour | mpv for Jellyfin; sites' own captions in web apps | 1.0 |
| Hearing | Mono audio, balance | PipeWire | 1.0 |
| Hearing | Dialogue boost | PipeWire filter | 2.0.0 |
| Motor | Slower key repeat, ignore accidental double presses, longer hold times | Hearth input service | 1.0 |
| Motion sensitivity | Reduce animations; no more than three flashes a second | Theme token; design rule | 1.0 |
| Cognitive | Fewer rows, bigger tiles, no autoplay | Rows-and-columns setting (section 18) | 1.0 |
| Cognitive | A separate simple layout | Layout option | 2.0.0 |

Rules for 1.0:
- Every new screen is checked with the high-contrast theme before it's done.
- Every control gets an accessible name and role (Qt's `Accessible.name` and `Accessible.role`), so the 2.0 screen reader doesn't reopen every screen. (Claude's proposal.)

## 15. What it looks like when it breaks

**Agreed** principle: solo projects forget the broken state, so Hearth designs it first.

- Every failure has a screen: one sentence, what to do next, an error code and a QR code.
- Error codes are readable, not numbers: `HEARTH-UPDATE-012`, `HEARTH-WIFI-003`.
- The QR code carries only the error code and the Hearth version, no personal data. Scanning it opens that error's help page on the phone, where "Report this" opens a pre-filled bug report from the phone. The box sends nothing (section 11).
- Diagnostics bundle: the user can save a cleaned-up log bundle to USB or send it to their phone, by choice each time.
- Recovery menu: opens after three failed boots, or by holding a remote button at power-on. Options: start the previous version, safe mode (shell with no apps), reset settings, factory reset, show diagnostics.
- Process rule: every piece of work has a "What it looks like when it breaks" section and isn't done until its failure screens exist.

## 16. Upstream etiquette (standing rule)

**Agreed.** Before contributing anything (code, a bug report, an issue) to another project:

1. Look for an AI policy: CONTRIBUTING, AI or LLM policy files, the code of conduct, the project website. Check again on every new pull from upstream.
2. If there's a policy, follow it.
3. If there's none, assume they don't want AI-assisted input, and keep our changes in our own repo.
4. Exception: a critical security flaw. Report it through the project's security channel and say that AI helped find it.

**Agreed (2026-11-22):** the rule covers contributions to someone else's code. Submitting Hearth's own work for a service, such as having `hearth-firefox-bridge` signed by Mozilla, isn't a contribution (the lead: "we can submit our stuff for signing. the rule is for contributions to someone else's code").

## 17. For the final plan review

- Video-decode tests on real hardware, left over from the [decode review](research/2026-11-19-hardware-video-decode-review.md):
  - hardware H.264 in Firefox, in whichever form it ships
  - which Intel driver build (free or non-free) we get. Partly answered by bifrost: Fedora's free build has no H.264 or HEVC, and RPM Fusion's non-free one adds them (section 5.3). Still to check on Hearth's own image.
  - AMD chips the review couldn't source
  - what YouTube, Twitch and Dropout send
- Feature review, a lighter Firefox kiosk: cut RAM and startup time per web app. Fewer content processes, the security cost of turning down site isolation (Fission), unneeded features off (Pocket, sync, studies, PDF viewer, spell check and so on), faster start and close, how long a closed web app stays warm, and whether each web app needs uBlock Origin now that AdGuard Home blocks ad domains for the whole box (section 13.1). Measure one YouTube web app on a 4 GB box before and after. Inputs: the Gecko research task (route 4) in [the lead's log](questions-and-tasks.md), and the RAM budget (section 5.5).
- Software that fills the gaps: where the lead's preferences (no Chromium, local only) leave holes, list candidate software and the trade-offs. Open to negotiation.
- Spike results: compositor (5.4; shell spike, headless; labwc chosen), typing with a test keyboard (6.4; done headless), native-messaging Back key (6.1; done headless: [shell spike results](research/2026-11-22-shell-spike-results.md)), KDE Connect typing into web apps (7), per-profile DNS (13.1).
- Agentic review plan (goal 6). The first research pass ([agentic review notes](research/2026-11-19-agentic-code-review-notes.md)) made no decisions; its sources were mostly blogs, none opened in full. Checked since ([agentic review checks](research/2026-11-19-agentic-review-checks.md)): Claude Code's limits (an allow-list, a git guard hook) aren't set up yet; branch protection is refused on a private repo with a free account; code checkers wait for code; mkosi pins Fedora with `Snapshot=`. Points for the review:
  - Rules files are advice; hooks, CI checks and branch protection enforce.
  - AI tools invent package names, and attackers register them. Every new dependency needs approval, and versions are pinned.
  - Pinning beyond npm: Fedora packages in mkosi builds (a fixed Fedora snapshot?) and the Flatpak versions we list.
  - Hosted code reviewers: review once the architecture is settled (lead's call, 2026-11-19). Nothing chosen. Background:
    - What they are: a company's service that reads your code. When a pull request opens, GitHub sends the changes to their servers and their tool leaves comments ("this could crash", "this looks like a leaked password"). Examples: GitHub Copilot review, CodeRabbit, SonarQube Cloud.
    - The local version: the same kind of checker on your own machine or in your own GitHub Actions run, so the code goes nowhere new. Examples: Semgrep CLI, gitleaks.
    - Why it matters less for Hearth (Claude's view, not decided): Hearth's code will be public GPL code anyway; the AI that writes it already sees all of it; "local only" is a promise about the TV user's data, not about how we build the software.
    - What still matters: a password or key committed by mistake gets copied to them too; the service needs repo permissions, so prefer read-and-comment only; check whether they keep the code or train on it.
    - To decide in the review: which hosted tools, if any, are acceptable, and which checks run locally. Needs the architecture first so the tools fit the stack (QML, C++, mkosi configs).
  - Playwright installs Chromium by default, which conflicts with the no-Chromium development rule.
  - Review tooling for QML and C++ wasn't researched (qmllint, Qt Quick Test and clang-tidy are guesses).
- Docs plan: user docs and contributor docs kept separate, organised with the Diátaxis framework (tutorials, how-to guides, reference, explanation).

## 18. Home screen and overlays

### The lead's wants (2026-11-22)

- One full-screen app at a time.
- Slick animations.
- A row or two of icons. The icons are the installed apps, which the user arranges; they aren't sorted into categories. One row shows at a time, with a little of the next row visible below it (if there is one), to invite scrolling down.
- A user setting for how many rows and columns are visible.
- Overlays: volume, festive celebrations, particle effects. Celebrations and particle effects appear on the home screen only.
- A user-set background.
- On the home screen, a status overlay: system clock, battery percentage (if the box has a battery), the current profile, a notification bell, internet connection state, and a profile switcher / power button.

### Agreed (2026-11-22), from Claude's proposals

- The clock and other fixed elements get burn-in protection: they dim or shift a few pixels over time, and a screensaver starts when the screen sits idle.
- Internet status shows the network state (wired, Wi-Fi, none). "No internet" shows only when something actually fails, such as lookups to the upstream DNS. No new connection is added to check. How to detect a failure is to be checked (AdGuard Home's own status is the first candidate).

### Claude's notes (proposed, not decided)

- **What the four TV design guides agree on** ([TV design principles](research/B1-tv-design-principles.md); Google, Apple, Samsung and LG's own pages): content first, with the fewest presses to reach it; everything legible from about 3 m; up, down, left, right, select and back must be enough; the TV is shared, so show who is signed in and keep personal details private; few screen levels; predictable, consistent navigation, with grids rather than diagonal layouts; a clear focus indicator, focused visibly different from selected, and a loading animation on long waits. LG adds at most two ideas per screen. Hearth's status overlay showing the current profile covers the "shared device" point.
- **Arranging apps with a remote:** focus an app, press Menu, choose "Move", move it with the arrows, press Select to drop it. Long-press "jiggle" modes (Apple TV) aren't reliable here, because NEC-protocol infrared remotes can't tell a held button from repeated presses (section 6).
- **App order:** each profile keeps its own order, since each profile is its own Unix user. Newly installed apps go at the end. Web apps count as installed apps. A kid profile shows only its allowed apps (section 13).

- **Row peek:** Samsung asks for a scroll hint when a list runs past one page ([TV focus and navigation](research/B2-tv-focus-navigation.md)). The peek does that job.
- **Rows-and-columns setting:** also gives the "fewer rows, bigger tiles" part of simple mode (section 14).
- **Animations:** none of the four TV guides gives durations or easing ([TV motion](research/B4-tv-motion.md)). Starting point: Material 3's standard easing, `cubic-bezier(0.2, 0, 0, 1)`, around 200 ms, tuned on the box. Animation speed is already a theme token. Reduced motion swaps slides and zooms for fades, as Apple's guide says, and turns particles off.
- **Particle effects:** Qt Quick has a particle module ([QtQuick.Particles](https://doc.qt.io/qt-6/qml-qtquick-particles-particle.html)). Limits:
  - No more than three flashes a second ([WCAG 2.3.1](research/A3-wcag22-numbers.md)). Fireworks are the classic failure.
  - Anything drawn over a playing full-screen app makes the compositor combine the two every frame, instead of sending the video straight to the screen. That costs graphics power on floor hardware (from memory; measure in the spike). The lead's call: celebrations and particles on the home screen only. The volume overlay is brief, so it's fine over apps.
- **Background:** theme packs already carry wallpapers (section 9). Photos sent from the phone (section 7) could be used too. Proposal: still images only in 1.0, since animated backgrounds cost RAM and graphics power.
- **Status overlay:**
  - Clock: a fixed element, the kind OLED makers warn can burn in ([OLED burn-in](research/A6-oled-burn-in.md)). Protection agreed above. Screensaver delay to be set; Samsung's TVs start theirs after 2 minutes.
  - Battery: shown only when a battery exists (section 2, optional hardware).
  - Internet state: agreed above. The reason: the "Wi-Fi sign-in page" check is off by default (section 2), and that check is how the box would tell "the internet works" apart from "connected to the network".
  - Profile switcher: switching ends the current session ([One Unix user per profile](decisions/0002-one-unix-user-per-profile.md)).
  - Power: sleep, restart, power off. What a kid profile may do here depends on the parental-controls question in the lead's log.
  - Bell: reads Hearth's own notification service (section 2).
  - Placement: inside the TV-safe margin. The guides give 54–60 px top and bottom and 80–96 px at the sides at 1920×1080 ([TV layout and safe area](research/B3-tv-layout-safe-area.md)).

## 19. System menu (Menu Contribution API)

**Agreed (2026-11-22):** in 1.0, used by Hearth's own screens and by web apps. A public SDK for app makers after 1.0.

The lead's design notes are in [App Menu Contribution API](research/2026-11-20-app-menu-contribution-api.md). In short:
- Pressing Menu opens one overlay drawn by Hearth: a system section always, and an app section when the app in front has contributed items.
- Apps send structured data only: no HTML, QML, scripts or graphics. Hearth owns presentation, navigation, focus and permissions.
- The app's name and icon come from its installed manifest, not from what it sends.
- Hearth enforces limits: menu depth, number of items, label length, update rate, payload size.
- The app reports state and requests changes. Hearth decides whether to honour a request.
- The API doesn't depend on any UI toolkit or compositor.

### What to borrow (Claude's proposal)

- **MPRIS**, Linux's standard D-Bus interface for "what's playing, play, pause, seek". Firefox reports web-page playback through it on Linux, since Firefox 75 ([bug 1353652](https://bugzilla.mozilla.org/show_bug.cgi?id=1353652), [Mozilla help](https://support.mozilla.org/kb/control-audio-or-video-playback-your-headset-or-keyboard)). Every media app gets play and pause in the menu without supporting Hearth's API. That covers open question 1 in the note for media apps.
- **GTK's exported actions (GAction over D-Bus)** for request-versus-confirmed state: an app publishes actions with a state, the host asks for a change, and the app reports the new state (from memory; to check).
- **[Qt Application Manager](https://doc.qt.io/QtApplicationManager/)**: Qt's framework in which a "System UI" owns the screen and each app is described by an installed manifest ([info.yaml](https://doc.qt.io/QtApplicationManager/manifest.html)). It's the closest existing match to these notes. GPL-3.0-only or commercial, not packaged in Fedora 45. For ideas, not as a base.
- **Web apps:** the page never talks to the menu. The Hearth extension contributes items, per site, over native messaging (section 6.1), because web pages are untrusted (section 12.2).
- **Parental controls:** the OS decides which items each profile sees (section 13).
- **Trusted identity:** the Flatpak app ID and its `.desktop` file, or the web-app definition (section 8.1).
- **A first outside app for the SDK after 1.0** (the lead's idea, 2026-11-22): Moonfin, the Jellyfin client the lead is considering (section 3), as a good candidate to add menu integration to. What bifrost recorded about Moonfin is in [bifrost: test boxes and TVs](research/2026-11-22-bifrost-optiplex-and-tv.md): not on Flathub, an unsigned `.flatpak` bundle with no update source, no full-screen setting, a TV mode. Nothing checked for Hearth yet.
- **Sites that use the Menu key** (Claude's note): Leanback for Dropout opens its own settings on the remote's Menu button. Once Hearth owns Menu, a site's or extension's menu has to arrive as items in the menu's app section, through the TV-navigation extension, instead.

## Sources

- KDE security advisory, KDE Connect device name and type spoofing (CVE-2025-32900): https://kde.org/info/security/advisory-20250418-2.txt
- KDE security advisory, KDE Connect pairing code brute force (CVE-2025-32898): https://kde.org/info/security/advisory-20250418-3.txt
- KDE security advisory, KDE Connect device impersonation in protocol 8 (CVE-2025-66270): https://kde.org/info/security/advisory-20251128-1.txt
- AdGuard Home v1.0.0 release: https://github.com/AdguardTeam/AdGuardHome/releases/tag/v1.0.0
- systemd-sysupdate D-Bus interface (experimental, systemd 257+): https://www.freedesktop.org/software/systemd/man/org.freedesktop.sysupdate1.html
- ParticleOS (systemd's own mkosi + sysupdate example OS): https://github.com/systemd/particleos
- WPE WebKit for embedded and set-top devices: https://wpewebkit.org/about/a-good-choice.html
- Ladybird status (pre-alpha; alpha aimed at 2026): https://urandom.io/blog/2026-05-09-the-browser-with-no-ad-sales-department
- AdGuard Home vs Blocky (RAM, features): https://dev.to/selfhostingsh/adguard-home-vs-blocky-which-dns-ad-blocker-2oj
- Plasma Bigscreen source (GitHub mirror): https://github.com/KDE/plasma-bigscreen
