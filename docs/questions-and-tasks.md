# The lead's questions and tasks

*Running log, started 2026-11-18. It holds only what's still open. When a question is answered or a task is finished, its line is deleted: the answer goes into the [design doc](design.md), research results into `docs/research/`, and locked-in decisions into decision records.*

## How to use this

- Questions are decisions only the lead can make. Answered in a design chat, each becomes an "Agreed" line in the design doc and is deleted here.
- Research tasks are small prompts for a fresh Sonnet chat. Sonnet is unreliable on big or loose tasks, so every prompt asks one narrow question, names the sources and the table to fill, and has a word limit (350–500 words):
  1. Paste the context block, then one prompt.
  2. The chat saves its answer to the Hearth project on claude.ai under the name the prompt gives.
  3. In a design session it's copied into `docs/research/` and folded into the plan. Ranking and proposing happen there, not in the research chat.
- Lead tasks are for the lead: decisions, hardware, accounts and reviews.

## Context block: paste this first in any research chat

The Hearth project on claude.ai already carries this as its description. In a chat inside that project, you can skip it.

```
CONTEXT: I'm the solo lead of Hearth, an open-source TV interface ("10-foot UI") for cheap Linux home-theatre PCs. I build with AI help and am not a trained programmer, so explain plainly and show real examples.
- Goal: a smart-TV replacement that runs like an appliance, as reliable as a basic fridge.
- Hardware floor: 4 GB RAM, x86-64, graphics with VP9 hardware decode: Intel 7th gen or newer, AMD Raven Ridge-class APUs (2018) or newer, NVIDIA supported. Raspberry Pi 4 is a design goal only.
- Optional hardware: on-board Wi-Fi, Bluetooth, USB infrared receivers, gamepads and HDMI-CEC are recommended and supported by default, but never assumed present; old laptops, desktops and mini-PCs often lack them.
- Platform: Hearth's own Qt 6 QML shell (GPL-3.0-or-later), borrowing parts from KDE Plasma Bigscreen (no Plasma underneath). One full-screen app at a time; Hearth draws the overlays and the Menu-button system menu, and apps add menu items through a Hearth API. Compositor: labwc. Heavy accessibility features (screen reader, zoom, colour filters) come in 2.0.0. OS image built with mkosi on Fedora 45, installed from an "Installer" boot profile running systemd-sysinstall, updated with systemd-sysupdate (A/B, read-only system, automatic rollback). Boots on UEFI only. Fedora's stock kernel, Secure Boot off by default in 1.0; drivers Fedora doesn't ship (NVIDIA, some Wi-Fi) are pulled at setup time for the hardware found.
- Hearth's own parts live in the OS image, never as Flatpaks; any other app can be a Flatpak.
- Each profile is its own Unix user (one active session at a time, optional PIN); logins are kept in oo7, the secret store.
- Apps: Jellyfin, Moonlight, and Firefox kiosk "web apps" (YouTube TV interface, Twitch, Dropout) with our own TV-navigation extension. One global key map for every app.
- Phone companion: KDE Connect. Ad and tracker blocking on the box: AdGuard Home, pulled from AdGuard's own releases and pinned.
- Hard rules: local only (no telemetry, no data collection; every network connection is named and switchable); security first; our own development doesn't build on Chromium or Electron (users may install anything); follow each upstream project's AI policy, and if there isn't one, assume they don't want AI-assisted contributions.

RULES FOR YOUR ANSWER
- Answer only the questions asked. You may give recommendations, marked as such. Don't decide
  anything.
- Tag every fact: [C] = you read the primary source (official docs, spec, source code) and it
  says this; [S] = secondary source (news, blog, forum); [M] = from memory.
- Give a link for every [C] and [S]. If you can't find something, write NOT FOUND. Never guess.
- Plain language. Define any technical term the first time. No praise, no filler.
- Stay under the word limit in the prompt.
- Save your answer to the Hearth project under the name the prompt gives.
```

## Open questions (decisions for the lead)

| Question | Why it matters | Where it's discussed |
|---|---|---|
| Moonlight and a Jellyfin client (the lead names Moonfin): in the image, or Flatpaks? (Firefox: in the image.) Later (the lead, 2026-11-22). Neither is in Fedora 45's own repos | Decides part of the codec plan and how they update | Design doc, sections 3 and 5.3 |
| How a PIN guards a profile: Hearth checks the PIN itself, or a root service sets the PIN as the Unix password? Later (the lead, 2026-11-22) | Fedora refuses passwords under 6 characters when users set their own | Design doc, section 13.2; [PIN as password](research/P3-pin-as-password.md) |
| Fonts: which open-source fonts ship in the image, and which is the default? | The first real screen needs one | Design doc, section 9.2; [open-source fonts](research/B5-oss-fonts.md) |
| Kirigami: does the fresh shell use any Kirigami (KDE) controls, or only its own? | Decides whether the theme needs its Kirigami bridge, and a dependency | Design doc, section 9 |
| Kernel lockdown default: off, on, or on except where an outside driver is needed (proposed)? | Lockdown would block drivers pulled at setup time | Design doc, section 5.6 |
| NVIDIA floor: Pascal (GTX 10) or Turing (GTX 16 / RTX 20)? | Depends on which cards NVIDIA's current driver still supports | Design doc, section 5.6 |
| Boot-time target | Shapes the image and startup work | Design doc, section 3 |
| Casting in 1.0: AirPlay and DLNA receivers as opt-in switches, or later? | Each one is a listening port | Design doc, section 7 |
| Deferred until the architecture is settled: review hosted code-reviewer options (which, if any, and what runs locally instead) | They send our code off the machine; background in the design doc | Design doc, section 17 |
| Paid streaming switch: in 1.0 or later? | Requires Google's copy-protection download | Design doc, section 8.4 |
| Rating names and 12-month expiry for Tested with Hearth | Public-facing wording | [Tested with Hearth](tested-with-hearth.md) |
| Error-code style (`HEARTH-WIFI-003`?) and help-page hosting | Every failure screen depends on it | Design doc, section 15 |
| Saved logins: what happens when the keyring is lost, and a "Saved logins" settings page to forget or reset logins (to scope) | The failure screen and Settings both depend on it | Design doc, sections 10 and 15; [Secret store: oo7](decisions/0001-keyring-oo7.md) |
| Parental controls without an admin PIN: section 13 relies on an admin PIN to leave a kid profile and to extend screen time, but PINs are now per profile and opt-in. How do parents lock a kid's restrictions? | Parental controls are a primary feature | Design doc, section 13; [Profile PINs](decisions/0003-profile-pins-opt-in-self-managed.md) |
| Feature review: make Firefox kiosk lighter. Which RAM and startup cuts are worth their security or compatibility cost? | One web app must fit the 4 GB RAM budget | Design doc, section 17 |
| Web-app engine: stock Firefox (kiosk, policies, extension, unsandboxed AutoConfig), or Bigscreen's Qt WebEngine (Chromium)? Only Qt WebEngine makes the page an item inside Hearth's own Qt scene; switching reverses the no-Chromium rule and Firefox-in-the-image | How native web apps can feel, ad blocking, codecs, security upkeep | Design doc, section 8.6; [Firefox outside customising](research/2026-11-19-firefox-outside-customising.md) |
| Branch protection for `main`: GitHub refuses it on a private repo with a free account. Make the repo public, pay for GitHub Pro (about $4 a month), or rely on local hooks until it goes public? Later (the lead, 2026-11-22). Meanwhile local hooks refuse force pushes to `main` and Claude asks before every push ([dev environment](dev-environment.md)) | Without it, nothing on GitHub stops a bad push to `main` | [Agentic review checks](research/2026-11-19-agentic-review-checks.md) |
| Network control for apps the user installs: is Flatpak's network permission enough, or does Hearth need per-app control (a firewall rule per app, a "this app wants to connect" prompt)? For a design session, not Sonnet. Shapes the settings screens | Local-only promise without auditing every app | Design doc, section 11; [allowlist draft](research/2026-11-19-network-allowlist-first-draft.md) |
| Firefox studies (Mozilla's experiments, delivered through Remote Settings): off, or allowed with the rest of Firefox's connections? Telemetry stays off either way | Studies can change Firefox's behaviour on boxes without a Hearth release | Design doc, section 2 (network defaults) |
| Screen mirroring: when to revisit | Lowest-priority phone feature | Design doc, section 7 |
| On-screen keyboard details, including word suggestions (wanted? kept per profile? an off switch?). The compositor is now chosen (labwc). Also from the spike: how the keyboard tells a real keyboard from a remote, so a real keyboard's typing isn't swallowed | Typing is the worst part of a TV | Design doc, sections 2, 6.2 and 6.4; [shell spike results](research/2026-11-22-shell-spike-results.md) |
| Signing `hearth-firefox-bridge` on addons.mozilla.org: Mozilla has no AI policy for add-ons. The ground rule says "follow each upstream project's AI policy; if it has none, stay out". Does it cover signing our own extension, or only contributions to their code? (The research chat and Claude read it as contributions only.) | Decides whether the bridge can be signed at all | Design doc, section 8.2; [unlisted signing](research/M1-unlisted-extension-signing.md) |

## Research tasks for a Sonnet chat

### L. Other languages after 1.0 (4 prompts, low priority)

Decided: English only until 1.0.0, without building 1.0 in a way that blocks other languages. Already known: Qt documents right-to-left layouts (`LayoutMirroring`) in [Right-to-left User Interfaces](https://doc.qt.io/qt-6/qtquick-positioning-righttoleft.html). Per-language keyboards go with series K. The "1.0 must do" checklist is for a design session once these are back.

#### L1. How Bigscreen's text gets translated

```
TASK: KDE Plasma Bigscreen (github.com/KDE/plasma-bigscreen) is Qt 6 QML. From its source and
KDE's developer docs (develop.kde.org):
1. Does its QML use KDE's i18n() (KI18n) or Qt's qsTr(), or both? Give file examples.
2. How does a KDE app that runs without the Plasma desktop load its translations (KI18n
   catalogs, KLocalizedContext)? What files does it need at runtime?
3. Where do KDE's translations come from (KDE's own translation teams), and does that need
   any network access on the device?

Under 400 words. Save to the Hearth project as L1-bigscreen-translations.md
```

#### L2. Font coverage and size per script

```
TASK: From packages.fedoraproject.org (Fedora 45), list the installed size of the Noto font
packages that cover: Latin/Cyrillic/Greek, Arabic, Hebrew, Devanagari, Thai, and Chinese/
Japanese/Korean (Noto Sans CJK). Also give the size of Fedora's default font set if Fedora
documents one.

Table: | Script | Fedora package | Installed size | Link |
Write NOT FOUND for any package you can't find.

Under 350 words. Save to the Hearth project as L2-font-sizes-per-script.md
```

#### L3. System language settings and their size

```
TASK: From Fedora's docs and packages.fedoraproject.org (Fedora 45), and systemd's man pages:
1. What are glibc langpacks (glibc-langpack-*, glibc-all-langpacks) and how big are they?
2. What does systemd-localed set (locale, keyboard layout), and where is it stored?
3. How does Fedora's "langpacks" mechanism pull in fonts and translations per language?

Under 400 words. Save to the Hearth project as L3-system-locales.md
```

#### L4. How Firefox picks a language

```
TASK: From Mozilla's docs or Firefox source (github.com/mozilla-firefox/firefox) and
packages.fedoraproject.org:
1. What do intl.locale.requested and intl.accept_languages do?
2. How are Firefox language packs installed (built in, extensions, Fedora's
   firefox-langpacks package), and can a policy set the language?
3. Do sites (YouTube's TV interface as the example) follow the Accept-Language header, or
   their own account setting? Use Google's own help pages for YouTube.

Under 400 words. Save to the Hearth project as L4-firefox-language.md
```

### F. Firefox memory settings (1 prompt, on hold)

Hold until "Firefox or Bigscreen's Qt WebEngine" is settled (open questions above). The other Gecko-browser questions are answered or dropped: stock Firefox can be shaped far enough from outside that a fork isn't needed ([Firefox outside customising](research/2026-11-19-firefox-outside-customising.md)).

#### F4. Firefox memory settings

```
TASK: Which Firefox prefs reduce memory use, and what does each cost?
Cover: dom.ipc.processCount and related content-process prefs, Fission (site isolation),
and any memory-saving prefs Mozilla documents.
From Mozilla's docs, Firefox source (searchfox.org or github.com/mozilla-firefox/firefox)
or Mozilla bug reports only.

Table: | Pref | What it does | Default | Security or compatibility cost | Link |
Then any published measurement of Firefox RAM with one YouTube tab (say who measured, on what
machine), or NOT FOUND.

Under 400 words. Save to the Hearth project as F4-firefox-memory-prefs.md
```

### G. Making web apps feel native (1 prompt, low priority)

Overlays and transitions around the page are compositor questions (series E, prompts E1 and E6). Hearth drawing its own browse rows from service APIs is ruled out for YouTube and only partly possible for Twitch ([service APIs](research/2026-11-19-service-apis-native-browse.md)). What's left is design reference for the Hearth API our extension offers to web apps (design doc, section 8.6).

#### G1. How TV platforms give web apps TV features

```
TASK: LG webOS and Samsung Tizen let web apps on their TVs handle the remote's Back key,
check the system theme, and show system overlays. From their official developer docs only
(webostv.developer.lge.com, developer.samsung.com/smarttv):
1. webOS: what webOSTV.js offers (list the functions that matter for keys, Back, theme,
   overlays or toasts).
2. Tizen: tizen.tvinputdevice and any other APIs for keys and system UI.

Table: | Platform | API | What it does | Link |

Under 400 words. Save to the Hearth project as G1-tv-web-app-apis.md
```

### D. Outside drivers pulled at setup time (3 prompts)

Plan: drivers Fedora doesn't ship (NVIDIA's own driver, some Wi-Fi) are fetched at first-boot setup for the hardware found. The network sources this adds go into the "record a real boot" spike, not research. Already answered: how Universal Blue and RPM Fusion ship NVIDIA ([NVIDIA and Wi-Fi drivers on image-based Fedora](research/D1-nvidia-on-image-fedora.md), folded into design doc section 5.3a).

#### D2. Kernel drivers as system extensions

```
TASK: From systemd's man pages (systemd-sysext, extension-release) and any project that ships
kernel modules this way (for example Flatcar Container Linux's NVIDIA extension):
1. Can a systemd system extension (sysext) add kernel modules to a read-only /usr?
2. How does a sysext declare which OS and kernel version it matches, and what happens at boot
   when the kernel has changed and the extension no longer matches?
3. Name any project that builds or downloads a kernel-module sysext per kernel, with a link.

Under 400 words. Save to the Hearth project as D2-driver-sysext.md
```

#### D3. Kernel lockdown and module signing

```
TASK: From the Linux kernel's own docs (docs.kernel.org: kernel lockdown, module signing) and
the kernel-lockdown man page:
1. In lockdown "integrity" mode, can the kernel load an unsigned module? What about with
   Secure Boot off?
2. How does the kernel decide which keys it trusts for module signatures (built-in keyring,
   machine owner keys / MOK, the .platform keyring)? Can a key created on the box be trusted
   without Secure Boot?
3. What do module.sig_enforce and CONFIG_MODULE_SIG_FORCE do?

Under 400 words. Save to the Hearth project as D3-lockdown-module-signing.md
```

#### D4. NVIDIA's supported GPUs and licence

```
TASK: From NVIDIA's own pages only (nvidia.com driver downloads and release notes, the Linux
driver README, and the licence that ships with the driver):
1. Which GPU generations does NVIDIA's current Linux driver branch support, and which
   (Maxwell, Pascal, Volta) moved to an older branch? Which branch, and until when?
2. Does the driver licence allow software to download and install the driver automatically
   on the user's machine? Does it allow a third party to redistribute it? Quote the lines
   (under 50 words each).

Table for 1: | Generation | Example cards | Current branch? | Last branch that supports it |
Link |

Under 450 words. Save to the Hearth project as D4-nvidia-support-and-licence.md
```

### C. KDE Connect security (1 prompt)

Already answered: the 2025 bugs and the version floor ([KDE Connect 2025 security bugs](research/C1-kdeconnect-2025-bugs.md), [QR pairing and old versions](research/C2-kdeconnect-qr-and-versions.md); design doc, section 12.3). Left: two advisories KDE published after that research.

#### C3. KDE Connect advisories of October 2026

```
TASK: KDE published two KDE Connect security advisories on 30 Oct 2026. Use KDE's own
advisories (kde.org/info/security) first, then NVD (nvd.nist.gov), then Fedora's update
system (bodhi.fedoraproject.org) for Fedora 45. For each advisory:
1. What the bug was, in one plain sentence.
2. Which app is affected (desktop, Android, iOS), which versions, and the first fixed version.
3. Is Fedora 45's kde-connect 26.08.3 affected, and has Fedora shipped a fix?

Table: | Advisory | CVE | The bug | App | Affected versions | Fixed in | Fedora 45 status | Link |
Write NOT FOUND for any cell you can't confirm.

Under 350 words. Save to the Hearth project as C3-kdeconnect-oct-2026-advisories.md
```

### X. KDE's AI policy (1 prompt)

Only KDE matters now: the one planned contribution is QR-code pairing in KDE Connect (design doc, section 12.3). Every other project's policy is checked when a contribution comes up, as the upstream etiquette rule requires (design doc, section 16).

#### X1. KDE's policy on AI-assisted contributions

```
TASK: Find KDE's current policy on contributions (code, bug reports, issues) made with AI
tools: KDE-wide, and for KDE Connect, Plasma Bigscreen and the Qt Project. Look at
community.kde.org, develop.kde.org, KDE's manifesto and governance pages, the projects'
repositories (CONTRIBUTING or similar files), and qt-project.org / wiki.qt.io.

Table: | Project | Policy exists? | What it says (quote, under 40 words) | Link | Date checked |
If there is no policy, write "none found" and list where you looked. Flag any outright ban.

Under 400 words. Save to the Hearth project as X1-kde-qt-ai-policy.md
```

## Spikes to add

Short hands-on tests found in design sessions. The labwc spike, agreed and planned, is in the design doc, section 5.4.

- Per-user DNS through the firewall: can nftables make each Unix user's lookups reach AdGuard Home from a different local address, so it sees separate clients? Fallback: one AdGuard Home per profile ([AdGuard per-profile check](research/2026-11-19-adguard-per-profile.md)).
- Firefox shaped from outside: unsandboxed AutoConfig works in the Firefox build the image ships; which built-in keys it can rebind (Escape, Backspace, Back); what kiosk mode does to context menus and dialogs ([Firefox outside customising](research/2026-11-19-firefox-outside-customising.md)).
- Profiles and sessions, in a test image ([how profiles run](design.md), section 13.2): does Fedora's greetd login config register sessions with logind; what is Fedora's `KillUserProcesses=` default (`systemd-analyze cat-config systemd/logind.conf`); do Firefox and a Flatpak app stop after `loginctl terminate-user` (`systemd-cgls`); do `useradd` and `userdel` work with `/usr` read-only.
- AdGuard Home as a service (design doc, section 13.1): AdGuard's own build runs as an unprivileged user with only the port-53 capability, next to systemd-resolved, with `--no-check-update`; record every connection it makes.
- Record a real boot (first egress test): boot the first image in a test VM, record every DNS lookup and connection (method in the [allowlist draft](research/2026-11-19-network-allowlist-first-draft.md)), and turn it into the repo's allowlist file. Replaces desk research for the base-system list.

## Summarising a chat for the design log

Paste this into any Hearth chat before closing it, and save the reply to the Hearth project on claude.ai. Bring it to a design session, where it's copied into `docs/research/`.

```
Summarise this conversation for Hearth's design log, so a fresh session can use it without the chat:
1. Decisions I made (one line each).
2. Facts you confirmed, with source links.
3. Things you were unsure about or couldn't confirm.
4. Open questions I still need to answer.
5. Anything here that conflicts with the context block at the top of docs/questions-and-tasks.md (paste it in if this chat can't see the repo).
Plain language, under 400 words. No praise, no filler.
```

## Lead tasks

| Task | Why | Effort |
|---|---|---|
| Read the [design doc](design.md) and mark each "proposed" line agree / disagree / discuss | Turns proposals into decisions | 1 hour |
| Get a real potato test box: an old 4 GB laptop or mini-PC with 7th-gen Intel or a Raven Ridge AMD APU. | The floor needs a test machine; the OptiPlex is above it | Cheap or free |
| Get a USB infrared receiver and a $10–15 air-mouse remote | Default key-map testing | About $25 |
| Try the test keyboard in a window with the air mouse (`spikes/shell/run.sh`, then a web app's search box) | All keyboard results so far are headless | 15 min |
| When the bridge is ready to sign: an addons.mozilla.org developer account and API keys | Unlisted signing needs them | 15 min |
| Paste the summary prompt (above) into the threat-model chat and bring the reply back | It's the only earlier chat not yet in the repo | 10 min |
| Decide the open questions above, a few at a time | Unblocks decision records | Ongoing |
