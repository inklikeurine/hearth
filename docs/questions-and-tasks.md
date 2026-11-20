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
- Platform: a fork of KDE Plasma Bigscreen's QML interface with Plasma removed (Qt 6, GPL). OS image built with mkosi on Fedora 45, installed from an "Installer" boot profile running systemd-sysinstall, updated with systemd-sysupdate (A/B, read-only system, automatic rollback). Fedora's stock kernel, Secure Boot off by default in 1.0; drivers Fedora doesn't ship (NVIDIA, some Wi-Fi) are pulled at setup time for the hardware found.
- Hearth's own parts live in the OS image, never as Flatpaks; any other app can be a Flatpak.
- Each profile is its own Unix user (one active session at a time, optional PIN); logins are kept in oo7, the secret store.
- Apps: Jellyfin, Moonlight, and Firefox kiosk "web apps" (YouTube TV interface, Twitch, Dropout) with our own TV-navigation extension. One global key map for every app.
- Phone companion: KDE Connect. Ad and tracker blocking on the box: AdGuard Home.
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
| Moonlight and the Jellyfin client: in the image, or Flatpaks? (Firefox: in the image) | Decides part of the codec plan and how they update | Design doc, sections 3 and 5.3 |
| KWin on its own, labwc (a small wlroots compositor), or Hearth as its own compositor (Qt Wayland Compositor, which makes every app's window an item in Hearth's scene)? cage is out | RAM, accessibility features, how fake keys get typed, whether our on-screen keyboard can type into every app | Design doc, sections 5.4 and 6.2 |
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
| Branch protection for `main`: GitHub refuses it on a private repo with a free account. Make the repo public, pay for GitHub Pro (about $4 a month), or rely on local hooks until it goes public? | Without it, nothing on GitHub stops a bad push to `main` | [Agentic review checks](research/2026-11-19-agentic-review-checks.md) |
| Network control for apps the user installs: is Flatpak's network permission enough, or does Hearth need per-app control (a firewall rule per app, a "this app wants to connect" prompt)? For a design session, not Sonnet. Shapes the settings screens | Local-only promise without auditing every app | Design doc, section 11; [allowlist draft](research/2026-11-19-network-allowlist-first-draft.md) |
| Firefox studies (Mozilla's experiments, delivered through Remote Settings): off, or allowed with the rest of Firefox's connections? Telemetry stays off either way | Studies can change Firefox's behaviour on boxes without a Hearth release | Design doc, section 2 (network defaults) |
| Screen mirroring: when to revisit | Lowest-priority phone feature | Design doc, section 7 |
| Deferred until the compositor is chosen: on-screen keyboard details, including word suggestions (wanted? kept per profile? an off switch?) | Typing is the worst part of a TV; the compositor decides what the keyboard can do | Design doc, sections 2 and 3 |

## Research tasks for a Sonnet chat

### P. One Unix user per profile (5 prompts)

Decided: each profile is its own Unix user, one session at a time, optional PIN set by the profile itself, boot picker unless one profile with no PIN ([One Unix user per profile](decisions/0002-one-unix-user-per-profile.md), [Profile PINs](decisions/0003-profile-pins-opt-in-self-managed.md), [Boot shows the profile picker](decisions/0004-boot-shows-profile-picker.md)). Not researched here, because they're design or test work: which services run once per box and which per user (design session); per-profile DNS filtering (spikes to add, below); RAM per session and switch time (measure in a spike); recovering a PIN through a paired phone (design session).

#### P1. Starting a session from a picker (greetd)

```
TASK: From greetd's own docs and man pages (git.sr.ht/~kennylevinsen/greetd) and systemd-logind's
man pages (freedesktop.org):
1. How does greetd start a user's session after the greeter (picker) hands it a user name?
   Which user does the greeter itself run as?
2. What do greetd's "initial_session" (autologin) and "default_session" settings do?
3. How does the new session get the seat (display, input devices, sound): which logind
   mechanism hands them over?

Under 400 words. Save to the Hearth project as P1-greetd-sessions.md
```

#### P2. Ending a session cleanly

```
TASK: From systemd's man pages only (loginctl, logind.conf, systemd-logind, user@.service):
1. What is the difference between "loginctl terminate-session" and "loginctl terminate-user"?
2. What does KillUserProcesses= do, and what is Fedora's default?
3. When a user's last session ends, what happens to user@.service and the processes it
   started (for example a Flatpak app or Firefox started from that session)? What does
   "linger" change?

Table for 1 and 2: | Command or setting | What it ends | Default | Link |

Under 400 words. Save to the Hearth project as P2-ending-sessions.md
```

#### P3. A PIN as the user's password

```
TASK: On Fedora 45, from the man pages of pam_unix, pam_faillock and authselect, and Fedora's
own docs:
1. How does pam_unix store a password hash, and which hash method does Fedora use by default
   (yescrypt or other)?
2. pam_faillock: what do deny=, unlock_time= and fail_interval= do, and what are Fedora's
   defaults?
3. Is there a minimum password length enforced by default on Fedora (pam_pwquality), which
   would block a 4-digit PIN? Name the setting.

Table: | Setting | What it does | Fedora default | Link |

Under 400 words. Save to the Hearth project as P3-pin-as-password.md
```

#### P4. Creating and deleting users on an image-based system

```
TASK: Hearth's /usr is read-only; /etc, /var and /home are writable. From systemd and Fedora
docs (freedesktop.org man pages: useradd, systemd-homed, homectl, userdbctl, sysusers.d):
1. Can useradd and userdel be used normally when /usr is read-only? Where do the user
   records go?
2. What is systemd-homed, is it packaged and supported on Fedora 45, and what does it change
   (encrypted home, portable user records)?
3. What does userdbctl show, and can it create users?

Table: | Tool | Creates users? | Where the record lives | On Fedora 45? | Link |

Under 400 words. Save to the Hearth project as P4-creating-users.md
```

#### P5. An admin profile with polkit

```
TASK: From polkit's own docs (the polkit man page, polkit.freedesktop.org or
github.com/polkit-org/polkit) and Fedora 45 packages:
1. What format do polkit rules use (JavaScript .rules files?), and where do they go?
   Which polkit version does Fedora 45 ship?
2. Show the documented example of a rule that allows members of one Unix group to perform
   one action without a password.
3. How does a D-Bus system service ask polkit whether the caller may do an action?

Under 400 words. Save to the Hearth project as P5-polkit-admin.md
```

### A. Theme presets and default look (7 prompts)

Facts the preset set will be built from. Background: [theming research](research/2026-11-19-theming.md).

Already decided, so these prompts don't research it:
- Settings are preview-first, then apply.
- A day/night toggle, off by default. On: Day theme and Night theme dropdowns, switching at two fixed times the user sets (no sunrise/sunset).
- A theme can have a light mode and a dark mode. They pre-fill the dropdowns, and the theme's default mode shows when the toggle is off.
- Custom fonts are not allowed in theme packs. Hearth ships a set of open-source fonts in the OS image, and theme authors pick from those.

#### A1. Appearance settings on TV platforms

```
TASK: List the appearance and accessibility display settings on four TV platforms:
Apple tvOS, Google TV, LG webOS, Samsung Tizen.
Use each maker's own support or user-guide pages.

For each platform, fill one table:
| Setting | What it changes | Options | Menu path |
Include: light/dark/automatic looks (and, for automatic, what triggers the switch: clock
times, sunset, or light sensor), accent colours, text size, bold text, high contrast,
colour filters / colour inversion, reduce motion, reduce transparency, focus highlight
style. Leave out settings that aren't about how the interface looks.

Under 500 words. Save to the Hearth project as A1-tv-appearance-settings.md
```

#### A2. Appearance settings on consoles and Roku

```
TASK: Same as A1, for four more platforms: Roku OS, Xbox (Series X|S dashboard),
PlayStation 5, Steam Big Picture mode.
Use each maker's own support pages.

For each platform, fill one table:
| Setting | What it changes | Options | Menu path |
Include: light/dark looks (and any automatic switching: what triggers it), themes or
backgrounds, accent colours, text size, high contrast, colour filters, reduce motion.
Leave out settings that aren't about how the interface looks.

Under 500 words. Save to the Hearth project as A2-console-appearance-settings.md
```

#### A3. The accessibility numbers from WCAG 2.2

```
TASK: From the WCAG 2.2 standard itself (https://www.w3.org/TR/WCAG22/) and its
"Understanding" pages, give the exact rule and numbers for these success criteria:
1.4.3 Contrast (Minimum), 1.4.6 Contrast (Enhanced), 1.4.11 Non-text Contrast,
2.4.7 Focus Visible, 2.4.11 Focus Not Obscured (Minimum), 2.4.13 Focus Appearance,
2.3.1 Three Flashes or Below Threshold, 2.3.3 Animation from Interactions.

Table: | Criterion | Level (A/AA/AAA) | The rule in one plain sentence | Exact numbers | Link |
Then one paragraph: how WCAG calculates a contrast ratio (the formula), with one worked
example (white text on #000000).

Use only w3.org pages. Under 450 words. Save to the Hearth project as A3-wcag22-numbers.md
```

#### A4. Minimum text size on a TV

```
TASK: What minimum text size do TV platform design guides recommend for text read from a
sofa? Check these sources only:
- Google's Android TV / Google TV design guidelines (developer.android.com)
- Apple Human Interface Guidelines, tvOS typography
- BBC's guidelines for TV apps (BBC GEL or BBC TV accessibility guidelines), if they exist

Table: | Source | Smallest body text | Unit used (pt, dp, px) | Screen resolution assumed |
Viewing distance assumed | Link |
Then convert each minimum to pixels on a 1920x1080 screen. Show the conversion, and say
NOT FOUND where a source gives no number.

Under 400 words. Save to the Hearth project as A4-tv-text-size.md
```

#### A5. Dark greys turning black on TVs (colour range)

```
TASK: "Crushed blacks": when a PC sends full-range RGB and the TV expects limited range (or
the reverse), what happens to near-black greys?
1. Explain the 0-255 (full) vs 16-235 (limited) ranges in plain words, with one example
   grey value and what the TV shows for it in each mismatch.
2. How does each Linux graphics driver let you set the range it sends?

Sources: the CTA-861 spec if reachable, TV maker support pages, and the Intel, AMD and
NVIDIA Linux driver docs (look for "Broadcast RGB" or similar).

Table for 2: | Driver | Setting name | Values | Default | Link |

Under 400 words. Save to the Hearth project as A5-tv-colour-range.md
```

#### A6. OLED burn-in from static menus

```
TASK: What do LG, Samsung and Sony say about static on-screen elements (logos, menus,
clocks) on their OLED TVs? Use each maker's own support pages only.

Table: | Maker | What they say about static images (one sentence, quoted) |
What the TV does to protect itself (for example pixel shift, logo dimming) | Link |

Under 350 words. Save to the Hearth project as A6-oled-burn-in.md
```

#### A7. Interface brightness on HDR TVs (low priority: run last)

The OptiPlex can't output HDR and the compositor isn't chosen yet, so this one can wait.

```
TASK: When a TV is in HDR mode, how bright should a normal (SDR) user interface's white be?
1. What does ITU-R Report BT.2408 say about "graphics white" or "reference white" in nits?
2. What do KDE Plasma (KWin) and GNOME (Mutter) do for SDR content brightness on HDR
   screens? Name the setting and its default, from their docs or source.

Table: | Source | What it says | Number (nits) | Link |

Under 350 words. Save to the Hearth project as A7-ui-white-on-hdr.md
```

### B. Design language and best practices (5 prompts)

What the TV makers' own design guides say. Each prompt uses only these four guides unless it says otherwise:
- Google TV / Android TV design guidelines (developer.android.com/design/ui/tv)
- Apple Human Interface Guidelines, tvOS (developer.apple.com/design/human-interface-guidelines)
- Samsung Smart TV design guide (developer.samsung.com/smarttv)
- LG webOS TV design guide (webostv.developer.lge.com)

#### B1. Stated design principles

```
TASK: What core design principles does each of these four TV design guides state?
Google TV / Android TV design guidelines, Apple Human Interface Guidelines (tvOS),
Samsung Smart TV design guide, LG webOS TV design guide. Use only the makers' own pages.

Table: | Source | Principle (their wording, short) | What it means in practice |
Link |
List only principles the guide states itself; don't add your own. Write NOT FOUND for a
guide you can't reach.

Under 500 words. Save to the Hearth project as B1-tv-design-principles.md
```

#### B2. Focus and navigation

```
TASK: What do these four TV design guides say about focus and remote navigation?
Google TV / Android TV design guidelines, Apple Human Interface Guidelines (tvOS),
Samsung Smart TV design guide, LG webOS TV design guide. Use only the makers' own pages.
Cover: how the focused item should look (outline, scale, glow, colour), how the arrow keys
move focus between items and rows, what Back does, whether lists wrap around at the ends,
and where focus starts on a new screen.

Table: | Source | Rule | Numbers given (for example scale factor, outline width) | Link |

Under 500 words. Save to the Hearth project as B2-tv-focus-navigation.md
```

#### B3. Layout and safe area

```
TASK: What do these four TV design guides say about screen layout at 1920x1080?
Google TV / Android TV design guidelines, Apple Human Interface Guidelines (tvOS),
Samsung Smart TV design guide, LG webOS TV design guide. Use only the makers' own pages.
Cover: the margin to keep clear at the screen edges (the "safe area" some TVs crop),
the layout grid (columns, gutters), and the base spacing unit.

Table: | Source | Edge margin | Grid | Spacing unit | Unit used (px, dp, pt) | Link |
Convert every number to pixels at 1920x1080 and show the conversion. Write NOT FOUND
where a guide gives no number.

Under 450 words. Save to the Hearth project as B3-tv-layout-safe-area.md
```

#### B4. Motion

```
TASK: What do these four TV design guides say about animation?
Google TV / Android TV design guidelines, Apple Human Interface Guidelines (tvOS),
Samsung Smart TV design guide, LG webOS TV design guide. Use only the makers' own pages.
Cover: recommended durations, easing (how a movement speeds up and slows down), and what
each guide says about reduced motion.

Table: | Source | Animation type (focus move, screen change, and so on) | Duration (ms) |
Easing | Link |
Then one line per source on reduced motion. Write NOT FOUND where a guide gives no number.

Under 450 words. Save to the Hearth project as B4-tv-motion.md
```

#### B5. Open-source fonts for TV screens

```
TASK: Facts only about these open-source font families: Inter, Noto Sans, Atkinson
Hyperlegible, Source Sans 3, Lexend, Red Hat Text, IBM Plex Sans, Fira Sans.
Use each font's own project page or repository, Google Fonts pages, and
packages.fedoraproject.org.

Table: | Font | Licence | Scripts covered (Latin, Cyrillic, Greek, others) | Weights |
Static files available (not only variable)? | Fedora package name | Legibility claims,
with who makes them | Link |
Don't rank them. Write NOT FOUND where a fact isn't on those pages.

Under 450 words. Save to the Hearth project as B5-oss-fonts.md
```

### C. Phone pairing with KDE Connect (2 prompts)

Already known, so not re-researched: pairing uses TLS, and from protocol version 8 a verification key shows on both devices. KDE's AI policy is covered by the AI-policies task.

#### C1. KDE Connect security bugs in 2025

```
TASK: For these three KDE Connect security bugs: CVE-2025-32898, CVE-2025-32900 and
CVE-2025-66270. Use KDE's own security advisories (kde.org/info/security) first, then
NVD (nvd.nist.gov) and the KDE Connect source (invent.kde.org or the GitHub mirror
github.com/KDE/kdeconnect-kde).

Table: | CVE | What the bug was, in one plain sentence | Which app (desktop, Android,
iOS) | Fixed in version | KDE advisory link | NVD link |
Write NOT FOUND for any cell you can't confirm.

Under 350 words. Save to the Hearth project as C1-kdeconnect-2025-bugs.md
```

#### C2. QR-code pairing and refusing old versions

```
TASK: Two yes/no questions about KDE Connect, each answered with evidence (source code,
release notes, or KDE's bug tracker bugs.kde.org):
1. Does any KDE Connect app (Android, iOS, desktop) support pairing by scanning a QR code?
   If no, link any open feature request.
2. Can the desktop side be set to refuse devices that use an older protocol version? Name
   the setting or code that does it, or say NOT FOUND.

Under 300 words. Save to the Hearth project as C2-kdeconnect-qr-and-versions.md
```

### E. Compositor choice: KWin, labwc, or Hearth as its own compositor (6 prompts)

A compositor is the program that draws all windows on screen. The candidates are KWin (KDE's, run without the Plasma desktop), labwc, and Hearth's own shell acting as the compositor through Qt Wayland Compositor (prompt E6), which would make every app's window, Firefox included, an item inside Hearth's QML scene with no change to the apps. cage is out: it shows one full-screen app at a time, has no layer-shell (overlays) and no input-method or text-input support, only fake key presses ([cage.c](https://github.com/cage-kiosk/cage/blob/master/cage.c), checked 2026-11-19).

#### E1. Overlays and switching apps

```
TASK: For KWin (run without plasmashell) and labwc, from their docs, source or issue
trackers:
1. Does it support the wlr-layer-shell protocol (lets a program draw panels and overlays)?
2. Can a layer-shell overlay appear on top of a full-screen app window?
3. Can a "home" program switch between full-screen apps and bring itself back to the front?
   Name the protocol or mechanism (for example foreign-toplevel management, KWin scripting).
4. KWin only: can a QML scene (a KWin script or effect, or another program) show a live,
   updating view of another app's window as an item, for example to animate it into a
   tile? Name the QML type or API.

Table: | Compositor | layer-shell? | Overlay over full-screen app? | Switch apps / return home:
how | Source links |

Under 400 words. Save to the Hearth project as E1-compositor-overlays.md
```

#### E2. Input protocols the compositor implements

```
TASK: Which of these does each compositor implement: KWin (without plasmashell) and labwc?
Compositor side only; don't research which apps support them.
- input-method (unstable v1 and v2)
- text-input (v1, v2, v3)
- virtual-keyboard (unstable v1)
- libei / EIS (emulated input)
- the RemoteDesktop portal (xdg-desktop-portal), and which portal backend provides it
Use each project's source code (protocol registrations), release notes or docs.

Table: | Compositor | input-method | text-input | virtual-keyboard | libei | RemoteDesktop
portal (backend) | Since version | Source links |

Under 400 words. Save to the Hearth project as E2-compositor-input-protocols.md
```

#### E3. Accessibility and HDR

```
TASK: For KWin (without plasmashell) and labwc:
1. Accessibility: does the Orca screen reader work with apps running under it? Is there
   built-in screen zoom? Colour filters (for colour blindness)?
2. HDR and colour management: is HDR output supported, and since which version?

Table: | Compositor | Screen reader | Zoom | Colour filters | HDR | Since version |
Source links |

Under 400 words. Save to the Hearth project as E3-compositor-a11y-hdr.md
```

#### E4. Memory use and what each one pulls in

```
TASK: For KWin (without plasmashell) and labwc:
1. RAM use when idle, from any published measurement. Say who measured, on what system,
   and when. Write NOT FOUND if there is none.
2. The libraries each one needs at runtime. For KWin: which KDE Frameworks and Plasma
   libraries does it need even without plasmashell? Use Fedora's package dependency lists
   (packages.fedoraproject.org, Fedora 45) and the projects' build files.

Table for 2: | Compositor | Fedora package | Main runtime dependencies | Source link |

Under 400 words. Save to the Hearth project as E4-compositor-footprint.md
```

#### E5. Upkeep, and who runs a TV shell on it

```
TASK: For KWin and labwc:
1. Who maintains it, latest release and date, and how many people contributed in the last
   12 months (from the repository's own statistics, if shown).
2. Is anyone running Plasma Bigscreen, or a similar TV or kiosk shell, on labwc or on KWin
   without plasmashell? Links to the project or write-up.

Under 400 words. Save to the Hearth project as E5-compositor-upkeep.md
```

#### E6. Qt Wayland Compositor as Hearth's own compositor

```
TASK: Qt Wayland Compositor (doc.qt.io, module QtWayland.Compositor) lets a Qt Quick program
be the Wayland compositor, showing each app's window as a QML item (WaylandQuickItem).
From Qt's own docs and source (code.qt.io or github.com/qt/qtwayland), for Qt 6.11 or later:
1. Which Wayland protocols does it implement for apps? Check xdg-shell, wlr-layer-shell,
   text-input (v2, v3), input-method, virtual-keyboard, xdg-decoration, viewporter,
   linux-dmabuf, presentation-time, fractional-scale, and anything for HDR or colour
   management.
2. Can it run directly on the display hardware with no other compositor (Qt's eglfs
   KMS/DRM backend), including choosing the display mode?
3. Does it support accessibility for the apps it hosts (screen reader), or say NOT FOUND.
4. Which shipping products or projects use it as their main compositor? Links.

Table for 1: | Protocol | Supported? | Since Qt version | Link |

Under 500 words. Save to the Hearth project as E6-qt-wayland-compositor.md
```

### K. On-screen keyboard (4 prompts; run after series E)

Decided: one Hearth keyboard that types into every app, several key arrangements, themeable, English only until 1.0. Which input protocols each compositor implements is prompt E2. Conflicts with Hearth's rules (a keyboard sees every password; suggestions stay on the box) are for the design session.

#### K1. The Wayland keyboard protocols

```
TASK: From the protocol definitions themselves (gitlab.freedesktop.org/wayland/wayland-protocols,
and wayland.app/protocols):
1. What do input-method (unstable v1 and v2), text-input (v1, v2, v3) and virtual-keyboard
   (unstable v1) each do, in one plain sentence each?
2. In text-input v3: how does an app tell the keyboard a text box was focused, and how does it
   mark a password field (content purpose / hints)?
3. What path does a typed letter take from an on-screen keyboard to a text box in an app,
   using input-method v2 + text-input v3?

Under 450 words. Save to the Hearth project as K1-wayland-keyboard-protocols.md
```

#### K2. Which apps accept text from an on-screen keyboard

```
TASK: Which text-input protocol versions do these support on Wayland: Firefox (GTK on
Wayland), Qt 6 apps, SDL3 apps (Moonlight uses SDL), and GTK4 apps? Use each project's own
source code or docs (searchfox.org or github.com/mozilla-firefox/firefox, code.qt.io,
github.com/libsdl-org/SDL, gitlab.gnome.org/GNOME/gtk).

Table: | App toolkit | text-input v1 / v2 / v3 | Since version | Source link |
Write NOT FOUND where you can't confirm.

Under 350 words. Save to the Hearth project as K2-apps-text-input.md
```

#### K3. Existing keyboards to borrow from

```
TASK: Facts only about these on-screen keyboards: Maliit, Qt Virtual Keyboard, Squeekboard,
wvkbd, and the keyboard in KDE Plasma Bigscreen (github.com/KDE/plasma-bigscreen).
Use each project's own repository and docs.

Table: | Keyboard | Licence | Language / toolkit | Last release and date | Draws itself with
(QML, GTK, other) | Can be driven by arrow keys / D-pad, not only touch? | Link |
Don't rank them. Write NOT FOUND where you can't confirm.

Under 400 words. Save to the Hearth project as K3-existing-keyboards.md
```

#### K4. Key arrangements on TVs and consoles

```
TASK: How do these platforms lay out their on-screen keyboard for a remote or gamepad:
Google TV (Gboard for TV), Roku, Apple TV, Xbox, PlayStation 5, Steam Big Picture.
Use each maker's own support or design pages; give a link to a picture where one exists.

Table: | Platform | Layout (QWERTY grid, alphabet grid, single row, other) | Extra keys
(space, delete, symbols, voice) | Link |

Under 400 words. Save to the Hearth project as K4-tv-keyboard-layouts.md
```

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

### U. UEFI on floor-era hardware (1 prompt)

Already answered ([boot assessment and BIOS](research/2026-11-19-boot-assessment-and-bios.md)): mkosi can build one image that boots on both UEFI and BIOS, but automatic rollback relies on systemd-boot, which is UEFI-only. The question left is whether "BIOS-only" boxes exist at the hardware floor at all.

#### U1. Do floor-era machines all have UEFI?

```
TASK: Hearth's hardware floor is Intel 7th gen (Kaby Lake, 2017) and AMD Raven Ridge (2018)
or newer.
1. Did PC boards and laptops of that era ship with UEFI firmware? What did Intel say about
   UEFI "classes" and removing legacy BIOS support (CSM), and when? Use Intel's own pages or
   UEFI Forum presentations.
2. On such machines, is legacy (CSM) mode a firmware setting a user can switch off again?
   Give one example from a major maker's support page (Dell, HP or Lenovo).

Under 350 words. Save to the Hearth project as U1-uefi-at-the-floor.md
```

### D. Outside drivers pulled at setup time (4 prompts)

Plan: drivers Fedora doesn't ship (NVIDIA's own driver, some Wi-Fi) are fetched at first-boot setup for the hardware found. The network sources this adds go into the "record a real boot" spike, not research.

#### D1. How image-based Fedora projects ship NVIDIA

```
TASK: From Universal Blue / Bazzite's own docs and repositories (github.com/ublue-os) and
RPM Fusion's docs (rpmfusion.org):
1. How do Universal Blue / Bazzite deliver NVIDIA's driver: prebuilt kernel modules in a
   separate image, akmods, or something else? Who signs the modules, and with what key?
2. How does RPM Fusion package NVIDIA's driver and Broadcom's "wl" Wi-Fi driver (akmod-*,
   kmod-*), and what does akmods do after a kernel update?

Under 450 words. Save to the Hearth project as D1-nvidia-on-image-fedora.md
```

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

Short hands-on tests found in design sessions.

- Per-user DNS through the firewall: can nftables make each Unix user's lookups reach AdGuard Home from a different local address, so it sees separate clients? Fallback: one AdGuard Home per profile ([AdGuard per-profile check](research/2026-11-19-adguard-per-profile.md)).
- Firefox shaped from outside: unsandboxed AutoConfig works in the Firefox build the image ships; which built-in keys it can rebind (Escape, Backspace, Back); what kiosk mode does to context menus and dialogs ([Firefox outside customising](research/2026-11-19-firefox-outside-customising.md)).
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
| Turn on two-factor login on GitHub. Protecting `main` and release branches waits on the branch-protection question above (private repos on a free account can't) | Supply chain (design doc, section 12.4) | 30 min |
| Get a real potato test box: an old 4 GB laptop or mini-PC with 7th-gen Intel or a Raven Ridge AMD APU | The floor needs a test machine; the OptiPlex is above it | Cheap or free |
| Get a USB infrared receiver and a $10–15 air-mouse remote | Default key-map testing | About $25 |
| Paste the summary prompt (above) into the threat-model chat and bring the reply back | It's the only earlier chat not yet in the repo | 10 min |
| Decide the open questions above, a few at a time | Unblocks decision records | Ongoing |
