# What's still open: an assessment

*Claude, 2026-11-22, at the lead's request ("Is there anything left that is an open question? Put together a comprehensive assessment"). Read from the design doc, the lead's log, decisions 0001–0015 and the shell spike's results. Everything ranked or recommended here is Claude's proposal; nothing in it is decided.*

*Update, same day: every research prompt in section 4 has been run and folded into the design doc; the lead has answered most of section 3's "before product code" decisions (design doc, section 2).*

*Tags: [T] = tested in the spike or checked on the dev computer today; [C] = read in a primary source; [M] = Claude's memory, unchecked.*

## The short answer

- **The shell's questions are answered.** On labwc, the home screen, system menu overlay, app switching, full-screen apps, the Back key through the Firefox extension, typing with an on-screen keyboard and key handling all work (78 of 78 headless checks; the lead ran it in a window). Building the system menu's app items, settings pages or home-screen extras is product work, not spike work. The lead is right about that.
- **The big unknowns are in the parts that make Hearth an appliance:** the OS image, updates and rollback, the installer, profiles and sessions, the per-profile DNS filter, and codecs and drivers on a read-only system. None of it has been tried. Decisions 0001–0007 and 0014 rest on research, not tests.
- **The tools for testing them are already on hand.** The dev computer has QEMU, libvirt and UEFI firmware (OVMF), with 320 GB free [T]. Fedora 45 packages mkosi 26, greetd 0.10.3, and oo7 0.7.0, the last as a beta [T].
- The rest is about 20 decisions only the lead can make (a few block product code), research prompts still waiting, and hardware checks the lead has put after the software work.

## 1. Unknowns a spike can answer, ranked

Ranked by how much of the plan rests on the answer, then by cost.

| # | Question | What rests on it | Where | Needs the lead's approval for |
|---|---|---|---|---|
| 1 | **Image, update, rollback.** Does a mkosi-built Fedora 45 image boot on UEFI? Does systemd-sysupdate install a new version beside the old one? Does boot counting roll back a broken version? Does a Hearth unit hold back the "good boot" mark until the shell reports in? Does the "Installer" boot profile (systemd-sysinstall, systemd 261) install to a blank disk? | Goal 1 (the fridge), sections 5.1–5.2, decisions 0005, 0006, 0014. systemd calls sysupdate's D-Bus interface experimental [C, section 5.1] | A virtual machine on the dev computer | mkosi in the box; using the host's QEMU and libvirt |
| 2 | **Profiles and sessions**, in that image. Does Fedora's greetd register sessions with logind? What is Fedora's `KillUserProcesses=` default? Do Firefox and a Flatpak app really stop after `loginctl terminate-user`? Do `useradd` and `userdel` work with `/usr` read-only? Does a profile's keyring unlock from a password sealed with `systemd-creds` (decision 0004), using oo7? | Decisions 0001–0004; switching profiles; deleting a profile | The same VM | greetd and oo7 in the test image |
| 3 | **Per-profile DNS.** Can nftables make each Unix user's lookups reach AdGuard Home from a different local address, so it sees separate clients? Fallback: one AdGuard Home per profile | Parental controls, a primary feature (section 13.1) | The same VM | AdGuard Home's pinned release (already agreed) |
| 4 | **AdGuard Home as a service.** Runs as its own user with only the port-53 right, next to systemd-resolved, with `--no-check-update`; what it connects to | Section 13.1 | The same VM | — |
| 5 | **Record a real boot.** Every DNS lookup and connection the first image makes, turned into the repo's allowlist file | Goal 3 (local only), section 11 | The same VM | A traffic-recording tool |
| 6 | **Codec add-on built on the box.** On first boot, build a system extension from RPM Fusion; rebuild it after an update; if that fails, the update still goes ahead. Whether it actually decodes needs hardware | Section 5.3, item 2 | VM for building; hardware for decoding | RPM Fusion repos in the test image |
| 7 | **The on-screen keyboard beyond Firefox.** Does it reach Flatpak apps (GTK4, Qt) and Moonlight (SDL3) through the sandbox? Does the "only for a described text box" rule hold for them? Do Flatpak apps go full screen and come home? | The agreed line "types into every app, including Flatpaks the user installs". Only Firefox has been tested | Dev box | Flatpak in the box, and test apps from Flathub |
| 8 | **Desktop services with no desktop.** Flatpak apps reach the secret store, file chooser and screenshots through "portals", which are picked per desktop name [M]. Which ones does a session named "Hearth" get? Does oo7's portal answer? | Decision 0001 (oo7); apps saving logins and opening files | Dev box | oo7 (Fedora packages `oo7-daemon` and `oo7-portal`, 0.7.0 beta) |
| 9 | **Firefox from outside, in Fedora's build.** Which `policies.json` path it reads; whether a force-installed signed extension skips every prompt; whether the system-wide native-messaging manifest works; whether unsandboxed AutoConfig works | How `hearth-firefox-bridge` gets installed (section 8.2) | Dev box (files in the box's `/usr`) | A signed extension to test with, such as uBlock Origin from addons.mozilla.org (a download) |
| 10 | **Driver add-ons as system extensions** (NVIDIA, Broadcom Wi-Fi), and what kernel lockdown does to them | Section 5.3a; the lockdown and NVIDIA decisions | VM, after research prompts D2 and D3 | — |

Smaller ones, low risk, can wait for product work: what Back does on real sites, which mark their menus differently; sites that use the Menu key; apps that hand off to another program and exit at once. Parked by the lead: private listening's lip sync (needs a spike, section 7), and KDE Connect typing, which won't work on labwc as it is [T, section 7].

**Claude's pick:** 1 and 2 together, as one "test image" spike in a VM. Every appliance promise depends on them, and they're the least tested part of the plan. 3–5 run in the same image afterwards. Meanwhile, 7–9 are small dev-box spikes, one approval each.

## 2. Hardware checks (the lead's order: after the software work)

| Check | Needs |
|---|---|
| labwc drawing straight to a real screen; its default renderer on floor graphics (spike check 8) | OptiPlex, then a 4 GB box |
| RAM: idle, and one YouTube web app playing, against the 4 GB budget (check 9, section 5.5). Feeds the lighter-Firefox review | A 4 GB box |
| Volume overlay over playing video drops no frames (check 10) | OptiPlex, then a 4 GB box |
| TV colour range under labwc (check 11). labwc's display-settings protocol has no colour-range setting [C, section 10] | A real TV |
| Hardware video decode: H.264 in Firefox, Intel free vs non-free driver, AMD (section 17) | OptiPlex, ThinkCentre |
| Sound to the TV first, and staying there when the screen sleeps (section 4) | A box on a TV over HDMI |
| Suspend, and waking from each remote; update timers catching up after a wake (section 4) | A real box |
| Boxes without Wi-Fi or Bluetooth (OptiPlex) | OptiPlex |

Still to get: a 4 GB floor box and a USB infrared receiver (lead tasks).

## 3. Decisions only the lead can make

### Needed before product code starts

| Decision | Claude's recommendation |
|---|---|
| When product code starts, and from which spike parts | The lead's call (the repo rule: product code waits until the lead says so) |
| Kirigami controls in the shell, or only Hearth's own? | None. The spike used only its own controls and theme tokens, with no gaps; it drops a dependency and the theme's Kirigami bridge |
| Web-app engine: Firefox, or Bigscreen's Qt WebEngine (Chromium)? | Close it as Firefox. Three agreed lines already point there: no Chromium, Firefox in the image, `hearth-firefox-bridge`. The Firefox memory research (F4) waits on this |
| Fonts: which ship, and the default | Atkinson Hyperlegible Next as the default (packaged in Fedora 45, made for low vision) with Noto Sans for wider coverage. From the research's facts; neither has independent legibility tests (section 9.2) |
| On-screen keyboard details: word suggestions (wanted? per profile? off switch?) | Wanted, off in password boxes (text-input v3 marks them [T]), per profile since each profile is its own user |
| Error-code style and help-page hosting | Readable codes as written (`HEARTH-WIFI-003`); help pages on the project's GitHub Pages |

### Needed before profiles are built

| Decision | Note |
|---|---|
| Parental controls without an admin PIN: how parents lock a kid's restrictions | A primary feature. polkit can make an admin a Unix group plus Hearth's rules (section 13.2), so the mechanism exists; who is admin and how a kid profile is left is the open part |
| How a PIN guards a profile | Parked by the lead ("later"). The research advises Hearth checking the PIN itself, not a root service setting it as the Unix password |
| Saved logins when the keyring is lost, and a "Saved logins" settings page | Needed for the failure screen and Settings |

### Needed before image, release or driver work

| Decision | Note |
|---|---|
| Kernel lockdown default | Research D3 first |
| NVIDIA floor: Pascal or Turing | Research D4 first |
| Boot-time target | Parked ("later"). The test image gives a first measurement |
| Firefox studies on or off | Telemetry stays off either way |
| Network control for apps the user installs | A design session, not research |
| Branch protection for `main` | Parked ("later"); local hooks meanwhile |
| Hosted code reviewers | Deferred until the architecture is settled, which it now largely is |
| Paid-streaming switch in 1.0 | — |
| Casting in 1.0 (AirPlay, DLNA) | — |
| Bundled apps (Moonlight, a Jellyfin client): image or Flatpak | Parked ("later") |
| "Tested with Hearth" rating names and expiry | Public wording |
| Lighter Firefox kiosk: which cuts | Needs the RAM measurement on hardware first |
| Screen mirroring: when to revisit | Parked |

### Proposals in the design doc waiting for a yes or no

The lead task "mark each proposed line agree / disagree / discuss" still stands. The ones product code will hit first: the keyboard's makeup (section 6.4), focus never follows the pointer (6.1), every control gets an accessible name (14), smallest text 24 px (9.2), home-screen notes (18), the menu's borrowing list (19), the bridge plan (8.2). Image work will hit: the codec plan (5.3), driver add-ons as system extensions (5.3a), hardware tiers (5.6), the "something is playing" test for updates (5.2), plain `useradd` (13.2), how AdGuard Home runs (13.1), uBlock Origin only where it's worth its RAM (13.1), phone pairing rules (12.3), engine-neutral web-app definitions (8.3).

## 4. Research prompts still waiting (in the lead's log)

| Prompt | Matters for | Priority (Claude's view) |
|---|---|---|
| D2 Kernel drivers as system extensions | Driver add-ons (spike 10) | Before driver work |
| D3 Kernel lockdown and module signing | Lockdown decision | Before driver work |
| D4 NVIDIA's supported GPUs and licence | NVIDIA floor | Before driver work |
| C3 KDE Connect's October 2026 advisories | Only while KDE Connect stays in the plan; phone work is parked | Low for now |
| X1 KDE's AI policy | Only the planned QR-pairing contribution | Low |
| L1–L4 Other languages | After 1.0; scoping only | Low |
| F4 Firefox memory settings | On hold until the web-app engine is settled | After that decision |
| G1 How TV platforms give web apps TV features | Design reference for the Hearth API for web apps | Low |

## 5. Answered by the spikes; pruned today

- Open question: "how the keyboard tells a real keyboard from a remote". Answered: it doesn't need to; keys are handled by what they are (section 6.2).
- Design doc: "the input service has to tell real keyboards from remotes" (6.4), "to test: whether Firefox passes `inputmode="none"` on" (6.4: it doesn't), "fake keys through the RemoteDesktop portal on KWin" (6.2), "revisit if the Back-key spike shows Firefox still takes keys" (8.5: it doesn't), bifrost's device grab as Hearth's plan (6.5).
- Lead task: an air-mouse remote is in hand; only the infrared receiver is left.

## What Claude couldn't confirm

- Whether mkosi 26 builds the image layout sysupdate and sysinstall expect without extra work: that's spike 1.
- The portal behaviour in spike 8 is from memory.
- oo7 0.7.0 is a beta in Fedora 45; whether it's stable enough for 1.0 is part of spike 2.
