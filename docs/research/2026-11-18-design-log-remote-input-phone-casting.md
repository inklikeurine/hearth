# Hearth design log: remote input, phone companion, casting (2026-11-18)

> Read with the later decisions. Since this chat: the floor is Intel 7th gen / AMD Raven Ridge (not 6th gen), KDE Connect is agreed as the phone base, CEC is a bonus, and remotes, Bluetooth and Wi-Fi are never assumed present (design doc, section 2).

## 1. Decisions you made
- Hardware floor: 4 GB RAM, x86-64, Intel 6th-gen graphics or newer. Pi 4 is a design goal only.
- Nothing else was decided in that chat. Everything else below is Claude's recommendation, not your decision.

## 2. Confirmed
- Kernel: CEC over DisplayPort works only with specific active adapters ([docs](https://docs.kernel.org/6.12/admin-guide/media/cec.html)). The kernel decodes common IR protocols; NEC can't tell held from repeated ([docs](https://docs.kernel.org/next/userspace-api/media/rc/lirc-dev-intro.html)). Standard key names: [table](https://kernel.org/doc/html/v6.4/userspace-api/media/rc/rc-tables.html).
- UxPlay: GPLv3 AirPlay mirror and audio; non-mirror video only for YouTube; no Apple DRM ([README](https://github.com/FDH2/UxPlay)).
- gmrender-resurrect: headless DLNA renderer ([repo](https://github.com/hzeller/gmrender-resurrect)).
- Real Cast needs Google-issued credentials; open workarounds are fragile ([Shanocast](https://github.com/coder0107git/shanocast), [GoCast](https://pkg.go.dev/github.com/tristanpenman/go-cast)).
- KDE Connect needs TCP/UDP 1714-1764 ([KDE](https://userbase.kde.org/Translations:KDEConnect/108/en)); the iPhone app is partial ([App Store](https://apps.apple.com/ca/app/id1580245991)).

## 3. Unconfirmed
- sysupdate rollback: secondary sources only; systemd's own docs not read.
- CVE-2025-66270 (KDE Connect): one vulnerability-database page; fixed versions unknown.
- KDE Connect typing on Wayland without Plasma.
- PCs lacking HDMI CEC: forum posts only.
- From memory, unchecked: iPhone browsers can't be share targets; non-HTTPS pages lose built-in crypto; unmapped IR buttons give only scancodes.
- Image size and 4 GB RAM use: unmeasured. UEFI on 6th-gen boards: assumed.

## 4. Open questions
- KDE Connect only, or plus a Hearth web app?
- AirPlay/DLNA casting in v1, as opt-in toggles?
- Which remotes ship first: IR, CEC, Bluetooth, air-mouse?
- How does a non-technical user install to the internal disk?

## 5. Conflicts and gaps
- KDE Connect: conflict. I recommended a web app instead. Your call.
- Firefox web apps: a web-app text box fills Hearth's own fields, not Firefox pages. KDE Connect keystrokes might; unverified.
- Fedora: UxPlay needs an H.264 decoder; Fedora strips patent codecs (hardware-video-decode-review.md says so for Mesa). GStreamer case unchecked.
- sysupdate: consistent, but needs UEFI, and Hearth must report "boot complete" or rollback misses a broken UI.
- 4 GB floor: no conflict. All Intel 4th-gen+ decode H.264 in hardware (project review); UxPlay's GStreamer path untested.
- Local-only: UxPlay's YouTube mode likely fetches from YouTube directly (unverified); every receiver port needs a name and a switch.
- No Chromium: Open Screen uses Chromium tooling; I advised skipping Cast.
- AdGuard Home: no conflict found.
- AI policy: not checked for any project above.
