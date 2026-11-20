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
| Starting point | Fork Plasma Bigscreen's UI and drop Plasma: no plasmashell, no desktop session underneath |
| Licence | Shell goes GPL (forced by forking Bigscreen) |
| Hardware floor | 4 GB RAM, x86-64, graphics with VP9 hardware decode: Intel 7th gen (Kaby Lake) or newer, AMD Raven Ridge-class APUs (2018) or newer, NVIDIA. Raspberry Pi 4 is a design goal only |
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
| Profiles | One Unix user per profile; one active session at a time, and a new login ends the old session. Any profile, the default one included, can have a PIN: off by default, set and removed by the profile itself, recovered only by a hint where possible ([Profile PINs](decisions/0003-profile-pins-opt-in-self-managed.md)). Keyring set up when a profile is created. Default profile made in the install flow. Boot shows the profile picker unless there is exactly one profile and it has no PIN ([Boot shows the profile picker](decisions/0004-boot-shows-profile-picker.md)). Recorded in [One Unix user per profile](decisions/0002-one-unix-user-per-profile.md); to be researched |
| Secret store | oo7, daemon and Flatpak portal ([Secret store: oo7](decisions/0001-keyring-oo7.md)). Each profile's keyring unlocks from a random password sealed with `systemd-creds`, made at profile creation ([Boot shows the profile picker](decisions/0004-boot-shows-profile-picker.md)) |
| On-box ad and tracker blocker | AdGuard Home |
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

- Compositor, the program that draws windows. gamescope is likely out (section 5.4). It must let Hearth's on-screen keyboard type into every app.
- On-screen keyboard details, including word suggestions. Waits for the compositor.
- Bundled apps, image or Flatpak. Firefox goes in the image. Are Moonlight and the Jellyfin client Hearth's own parts, or ordinary Flatpaks? Decides part of the codec plan (section 5.3).
- Kernel lockdown default: on, off, or on except on boxes that need an outside driver (section 5.6).
- NVIDIA floor: which generations count as supported, given NVIDIA's driver support (section 5.6).
- BIOS-only boxes: needs research first (section 5.2).
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

## 5. Platform

### 5.1 Base distro: Fedora 45 (Agreed)

mkosi builds from Fedora, Debian, Arch, openSUSE and others, so the base is a separate choice. Reasons for Fedora 45:

- Bigscreen's code needs Qt 6.11 or newer; Fedora ships it, Debian 13 doesn't.
- Its systemd is new enough for systemd-sysupdate's D-Bus interface (added in systemd 257), which Settings → Updates would use.
- The dev container is already Fedora.
- `systemd-sysinstall`, the installer tool, needs systemd 261 or newer. Fedora 45 ships 261.2, Fedora 44 ships 259.8 ([Installer](decisions/0005-installer-boot-profile-sysinstall.md)).

Risk: systemd marks the sysupdate D-Bus interface experimental, so it can change between versions without notice. Pin the systemd version per release and test the update path in CI.

### 5.2 Updates

| What | How it updates | How often |
|---|---|---|
| The OS image (kernel, Hearth shell, services) | systemd-sysupdate, A/B, signed | Weekly, plus urgent security fixes |
| Apps the user installs | Flatpak from Flathub | Daily check |
| Firefox | In the image (section 2) | With the OS, plus an image for every Firefox security release |
| Moonlight, Jellyfin client | In the image or Flatpak; open (section 3) | With the OS, or daily |
| Web-app definitions and their extensions | Hearth's own signed channel (section 8) | When the lead publishes; pinned versions |
| Codecs | Follows the OS (section 5.3) | With the OS |

- Rollback: systemd-boot boot counting. A new version gets three tries; if it never reaches "healthy", the box boots the old one and shows a message.
  - "Healthy" means the home screen works: Hearth reports a good boot only once the shell is up and responding. Otherwise a broken shell on a working system counts as a good boot and rollback never happens. Confirmed in systemd's docs: a Hearth unit ordered before `boot-complete.target` holds back the "good" mark until the shell reports in over D-Bus, the pattern systemd's own walkthrough uses ([boot assessment and BIOS](research/2026-11-19-boot-assessment-and-bios.md)).
  - Needs UEFI: systemd-boot doesn't run on BIOS-only machines. mkosi can build one image that boots on both, but rollback on BIOS would depend on GRUB doing the counting ([boot assessment and BIOS](research/2026-11-19-boot-assessment-and-bios.md)). To research (prompt U1 in the lead's log): whether floor-era machines all have UEFI, so "BIOS-only" just means UEFI switched off.
- Applying: download in the background, apply on a restart at an idle time, never during viewing.
- Hosting: sysupdate reads plain files over HTTPS (an image plus a signed checksum list), so any static host works: GitHub Releases or the user's own mirror.
- User control: pause, delay, or point at a mirror (section 11).

### 5.3 Codecs

The lead asked whether codecs we can't ship can be layered in. They can. On a sysupdate image, layering means a system extension: a small add-on image laid over the read-only system. If Hearth builds and hosts a codec extension, though, Hearth is distributing the codecs, which is what we're avoiding.

**Proposed**, in order of preference:

1. Flatpak apps (anything the user installs, and the bundled apps if they end up as Flatpaks) get codecs from Flathub's runtimes, so Flathub does the distributing. From the [video-decode review](research/2026-11-19-hardware-video-decode-review.md): since the Freedesktop 25.08 runtime, codecs come in a `codecs-extra` extension that installs automatically; the old "install ffmpeg-full" advice is outdated. Whether Firefox gets hardware H.264 from it is unconfirmed. Plan: install `codecs-extra` and the Intel video-driver extension at build time, and add a build test that fails if H.264 isn't hardware-decoded on the reference Intel box.
2. Everything in the image (Hearth's own parts, and the bundled apps if they live there). Outside Flatpak, Fedora strips H.264 and HEVC from its AMD video driver and from FFmpeg. The box builds its own codec extension on first boot from RPM Fusion and rebuilds it after each OS update, so Hearth distributes nothing. If the rebuild fails, the OS update still goes ahead and video falls back to CPU decoding with a notice. Codecs never block an update.

### 5.3a Hardware add-ons, pulled at setup time

**Agreed:** anything only some boxes need is fetched during first-boot setup for the hardware found, not shipped in every image, so Hearth never redistributes it. This covers codecs (item 2 above), NVIDIA's own driver, and Wi-Fi drivers Fedora doesn't ship (Broadcom `wl`, for example).

- How (**proposed**): each add-on is a system extension built or downloaded on the box for its exact kernel and OS version, and rebuilt after each OS update.
- A failed add-on never blocks an update. The box falls back with a notice: software video decoding, or the open-source NVIDIA driver.
- Each add-on source (RPM Fusion, NVIDIA) gets an entry on the connection allowlist (section 11).
- To research: how image-based Fedora projects (Universal Blue, Bazzite) handle NVIDIA, and whether the box can build driver add-ons on a read-only system. Task in the lead's log.

### 5.4 Compositor

gamescope is probably out: it needs Vulkan, which a lot of potato-class graphics lack or only half-support. Bigscreen's code expects KWin and uses layer-shell, which both candidates below support.

| Candidate | For | Against |
|---|---|---|
| KWin on its own (no plasmashell) | Bigscreen already targets it; colour-blindness filter and zoom built in (accessibility) | Heavier; pulls in more of KDE |
| labwc (small wlroots compositor) | Light; simple | We'd write focus switching and overlays ourselves |
| Hearth as its own compositor (Qt Wayland Compositor) | Every app's window, Firefox included, becomes an item in Hearth's QML scene: animate it, scale it into a tile, layer overlays, with no change to the app. One process instead of two | Hearth takes on the whole compositor job: display output and modes, HDR, input devices, keyboard input into apps, screen-reader support. Protocol support unconfirmed |

cage is out (checked in its source, 2026-11-19): one full-screen app at a time, no layer-shell for overlays, and no input-method or text-input support, so the on-screen keyboard could only send fake key presses. Research prompts for KWin vs labwc: series E in the lead's log.

Spike: run the forked shell on KWin and on labwc, on the OptiPlex and on a 4 GB box; measure RAM, and check app hand-off and return home.

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

## 6. Remotes and input

**Agreed:** air-mouse, infrared and Bluetooth remotes are first-class and all ship with default key maps; CEC is a bonus. None is assumed present: setup and recovery work with whatever input is plugged in (section 2). With default maps most remotes need no setup; the "press each button" learning wizard is optional.

**Agreed:** one global key map. The user maps buttons to Hearth actions (Back, Home, Select, Play/Pause…) once, and each button means the same thing in every app. Making every app obey that map is Hearth's job.

Confirmed from kernel docs (remote-input chat):

- The kernel decodes the common infrared remote protocols itself.
- NEC-protocol remotes can't tell a held button from a repeated press, so "hold to do X" isn't reliable with them.
- CEC over DisplayPort only works with specific active adapters.

### 6.1 Back, Esc and Backspace

The lead keeps fighting Firefox over these keys in Leanback for Dropout. The same keypress means different things to Firefox (Esc leaves full screen), to the site (Esc closes a menu) and to the remote's maker (some remotes send Backspace or BrowserBack for Back).

**Proposed:** apps never see the raw key.

1. Hearth's input service turns every remote button into a Hearth action (Back, Home, Select, Up, …) before any app gets it.
2. For web apps, the action goes to the Hearth TV-navigation extension over Firefox's native messaging (a supported channel from a local program to an extension), not as a fake keypress. The extension decides: close the site's menu, go back a page, or leave the app. Firefox never sees Esc, so full screen isn't affected.
3. For native apps, an app adapter turns the global actions into whatever that app understands (one needs Esc to go back, another Backspace). Users never see or edit adapters. Adapters come from the app's Tested with Hearth entry.
4. Home always goes home; it's never passed to an app.

Spike: native messaging latency (it must feel instant), and whether Firefox swallows any key before an extension sees it.

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
- How fake keys get typed depends on the compositor (section 5.4): the RemoteDesktop portal on KWin, or the kernel's uinput under any compositor.
- Security: this service can type anything into anything. It gets no network access, accepts input only from local devices and KDE Connect, and logs nothing that's typed.

### 6.3 Remapping after setup

Settings → Remotes → pick the remote → a drawing of a generic remote with the button highlighted → "press the button you want for Back". One map for everything, no per-app overrides, and a "reset to defaults" option. Works with a remote, a phone or a keyboard.

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
  - Spike: does KDE Connect typing reach a Firefox web app under our compositor, without Plasma?
  - That chat recommended a Hearth phone web page instead. KDE Connect stays the choice; the web page is the fallback if the spike fails.
- AirPlay: UxPlay (GPL-3) mirrors the screen and plays audio from iPhones, but not Apple-protected video. It needs an H.264 decoder, which ties into the codec issue (section 5.3).
- DLNA ("play to"): gmrender-resurrect is a headless receiver.
- Google Cast: a real receiver needs credentials issued by Google, and the open workarounds are fragile. Skip it.
- Every receiver is a listening port, so each gets a name and a switch (section 11).

Pairing: section 12.3.

## 8. Web apps

### 8.1 User-built web apps

A web-app definition is a small file: name, icon, URL, user agent (spoofed if a site only gives its TV interface to TVs), extensions, adapter (normally the TV-navigation extension), and which profiles may use it. Settings → Apps → "Add a web app" builds one with the remote. The built-in ones (YouTube, YouTube Music, Twitch, Dropout) use the same format.

### 8.2 Extensions and their update path

- Release Firefox only installs extensions signed by Mozilla. That covers both cases the lead wants: extensions listed on addons.mozilla.org, and extensions in a GitHub repo that Mozilla signs as "unlisted" (Leanback for Dropout works this way).
- Each definition pins an extension version and its checksum. A new version reaches boxes only when a new definition is published on Hearth's web-app channel, never by auto-installing the latest release (section 12.4).
- Advanced users can add any signed extension from a URL, with a warning that it updates on its own schedule.

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

Revisit if the Back-key spike shows Firefox still takes keys we need, or one Firefox web app doesn't fit the 4 GB RAM budget. A WPE-based launcher is then the next candidate.

### 8.6 Later: our own browser, and a Hearth API for web apps

*Exploration from November 2026, scoped as research in [the lead's log](questions-and-tasks.md).*

- Our own lightweight Gecko browser keeps Firefox extension support but brings security upkeep:
  - Forking Firefox (LibreWolf, Zen) means rebuilding on every Firefox release: every four weeks, sooner for security fixes.
  - Embedding Gecko in QML has one known example, Sailfish OS, whose small team has spent years catching up to Firefox versions Mozilla had already dropped.
  - Middle route: stock Firefox changed from outside with policies, prefs, its own style sheet and AutoConfig. Checked 2026-11-19: this goes far enough that a fork isn't needed. AutoConfig with its sandbox off gives a startup script full browser rights, so it can rebind the browser's keys and change its interface without a rebuild. It can break on Firefox updates, which the pinned, tested image catches ([Firefox outside customising](research/2026-11-19-firefox-outside-customising.md)). It can't make the page an item inside Hearth's Qt scene. Open question: Firefox, or Bigscreen's Qt WebEngine (Chromium), which can.
- A Hearth API for web apps: a small script our extension adds to every web app for consistent Back handling, theme colours, focus style and overlay requests. LG webOS ships a similar JavaScript library for its web apps. Works with the kiosk route.
- Native screens from service APIs: Hearth draws the browse screens and the page only plays video. Easy for Jellyfin, partial for Twitch, hard for YouTube (Google key, quotas, terms), not possible for Dropout.

## 9. Theming

Theming is built in from the start, because adding it later means finding every hard-coded colour. The forked Bigscreen code moves onto theme tokens before any new features.

| Layer | What users change | Where | Ships in |
|---|---|---|---|
| 1. Tokens | Colours, fonts, text size, corner roundness, spacing, animation speed, focus highlight style, UI sounds | Settings → Appearance with live preview; presets plus simple sliders (colour pickers are hard to use with a remote) | 1.0 |
| 2. Theme packs | Tokens, wallpapers, icons and sounds in one folder; fonts are picked from the open-source fonts in the image, never shipped in a pack | Import from USB or from the phone; share online | 1.0 |
| 3. Layout skins | Rearrange the home screen (rows, grid, sidebar) | QML files, which are code, so developer mode only | Later |

- Theme packs are data only, never code, so a downloaded theme can't run anything.
- Accessibility themes (high contrast, large text) are ordinary themes. An automatic test checks every built-in theme against WCAG AA contrast rules.
- Web apps take the focus highlight and overlay colours from the theme; the sites themselves are unchanged.

## 10. Settings from the remote

Bigscreen's settings pages are tied to Plasma. Rebuild them on system interfaces and KDE libraries that don't need Plasma.

Pages for optional hardware the box doesn't have (Wi-Fi, Bluetooth, CEC, gamepads, infrared) are hidden or say what's missing (section 2).

| Page | What's on it | Built on |
|---|---|---|
| Network | Ethernet, Wi-Fi list if the box has Wi-Fi, password entry (phone typing helps), hostname | NetworkManager (NetworkManagerQt) |
| Bluetooth | Only on boxes with Bluetooth. Pair remotes, controllers and headphones; discoverable only while this page is open | BlueZ (BluezQt) |
| Display | Resolution, refresh rate, overscan / screen fit, HDR where possible | The compositor's output settings |
| Sound | Output (TV, headphones, analog), volume limits, dialogue boost, mono audio | PipeWire / WirePlumber |
| Remotes | Remap (section 6.3), learning wizard, key-repeat speed | Hearth input service |
| Phone | Pair or unpair; what the phone is allowed to do | KDE Connect |
| Apps | Installed apps, "Tested with Hearth" list, add a web app | Flatpak and web-app definitions |
| Profiles and parental controls | Section 13 | Hearth profile service |
| Appearance | Section 9 | Theme tokens |
| Accessibility | Section 14 | Several |
| Privacy and network activity | Section 11 | Hearth and the firewall |
| Updates | Status, pause or delay, mirror, roll back to the previous version | systemd-sysupdate D-Bus |
| Time and language | Time zone, language, region | systemd timedated / localed |
| Power | Sleep timer, wake on remote, what happens after a power cut | systemd-logind |
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
- Secure Boot is off by default in 1.0 (**Agreed**). Kernel lockdown default: open (section 5.6). BIOS-only boxes need research (section 5.2).

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
- KDE Connect today: encrypted connections (TLS), and protocol version 8 shows a verification key on both devices during pairing. Recent flaws, both fixed in protocol version 8:
  - 2025: device identity could be spoofed with fake network broadcasts (CVE-2025-32900).
  - A pairing flaw showed a verification key built from an old certificate (CVE-2025-32898).
- **Proposed:**
  - Require protocol version 8; refuse older devices.
  - Pairing only works while Settings → Phone → Pair is open; the window closes after two minutes.
  - Show the verification key large on the TV.
  - Unpair from the TV or the phone.
  - To check: whether KDE Connect supports QR-code pairing. If not, it's a candidate contribution, only if KDE's AI policy allows.

### 12.4 Supply chain

If a box automatically installs the newest release from a GitHub repo (an extension, say), and that account is taken over or a bad change slips into a release (an AI agent's mistake included), every box installs it within a week. Mozilla's signing doesn't stop this: unlisted signing is automatic, not human-reviewed.

Fix: pin versions and checksums for everything pulled from outside (extensions, app bundles downloaded from GitHub, Flatpaks we list). A new version reaches boxes only through a release the lead approves. Plus two-factor login and protected release branches on the lead's GitHub.

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

## 14. Accessibility

| Need | What Hearth does | Built on |
|---|---|---|
| Blind and low-vision users | Screen reader for every Hearth screen, switchable from the first boot screen (hold Back for 5 seconds, as many TVs do) | Qt accessibility → AT-SPI → Orca |
| Low vision | Text size, high-contrast themes, zoom | Theme tokens; compositor zoom |
| Colour blindness | Colour filters; never colour as the only signal | Compositor filter; design rule |
| Deaf and hard of hearing | Captions on by default if chosen once; caption size and colour | mpv for Jellyfin; sites' own captions in web apps |
| Hearing | Mono audio, dialogue boost, balance | PipeWire filters |
| Motor | Slower key repeat, ignore accidental double presses, longer hold times | Hearth input service |
| Motion sensitivity | Reduce animations | Theme token |
| Cognitive | Simple mode: fewer rows, bigger tiles, no autoplay | Layout option |

Rule: every new screen is checked with the screen reader and the high-contrast theme before it's done.

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

## 17. For the final plan review

- Video-decode tests on real hardware, left over from the [decode review](research/2026-11-19-hardware-video-decode-review.md):
  - hardware H.264 in Firefox, in whichever form it ships
  - which Intel driver build (free or non-free) we get
  - AMD chips the review couldn't source
  - what YouTube, Twitch and Dropout send
- Feature review, a lighter Firefox kiosk: cut RAM and startup time per web app. Fewer content processes, the security cost of turning down site isolation (Fission), unneeded features off (Pocket, sync, studies, PDF viewer, spell check and so on), faster start and close, how long a closed web app stays warm, and whether each web app needs uBlock Origin now that AdGuard Home blocks ad domains for the whole box (section 13.1). Measure one YouTube web app on a 4 GB box before and after. Inputs: the Gecko research task (route 4) in [the lead's log](questions-and-tasks.md), and the RAM budget (section 5.5).
- Software that fills the gaps: where the lead's preferences (no Chromium, local only) leave holes, list candidate software and the trade-offs. Open to negotiation.
- Spike results: compositor (5.4), native-messaging Back key (6.1), KDE Connect typing into web apps (7), per-profile DNS (13.1).
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
  - Automated cleanup passes on forked Bigscreen code will cause merge conflicts with upstream.
- Docs plan: user docs and contributor docs kept separate, organised with the Diátaxis framework (tutorials, how-to guides, reference, explanation).

## Sources

- KDE security advisory, KDE Connect identity spoofing (CVE-2025-32900): https://kde.org/info/security/advisory-20250418-2.txt
- KDE Connect improper authentication (CVE-2025-32898): https://www.tenable.com/cve/CVE-2025-32898 and https://bugs.kde.org/show_bug.cgi?id=501352
- systemd-sysupdate D-Bus interface (experimental, systemd 257+): https://www.freedesktop.org/software/systemd/man/org.freedesktop.sysupdate1.html
- ParticleOS (systemd's own mkosi + sysupdate example OS): https://github.com/systemd/particleos
- WPE WebKit for embedded and set-top devices: https://wpewebkit.org/about/a-good-choice.html
- Ladybird status (pre-alpha; alpha aimed at 2026): https://urandom.io/blog/2026-05-09-the-browser-with-no-ad-sales-department
- AdGuard Home vs Blocky (RAM, features): https://dev.to/selfhostingsh/adguard-home-vs-blocky-which-dns-ad-blocker-2oj
- Plasma Bigscreen source (GitHub mirror): https://github.com/KDE/plasma-bigscreen
