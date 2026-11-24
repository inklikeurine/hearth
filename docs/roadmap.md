# Roadmap to 1.0 (proposed)

*Claude's proposal, 2026-11-23, rebuilt from the lead's answers that day: milestones are GitHub milestones, used to track how complete a feature is and whether it's ready to ship; milestones go part by part; every milestone gets broken into tasks; the grid shows which open decision blocks which piece of work. The order of the parts, the grid and the tasks are Claude's proposal. Nothing here is decided except those answers.*

## How it works

- **Parts and components** come from the [design doc's component map](design.md#3-component-map). Each part is one milestone. A milestone is done when every component in its part reaches "done" and its "Done when" checks pass.
- **Grid cells** say how far a component gets in each milestone:
  - **done**: finished for 1.0, including its failure screens.
  - **stub**: the least another part needs to build and test against. For example, the OS image milestone logs one fixed user in automatically, because the real login screen comes with Profiles.
  - **+ something**: added to a done component when a later part needs it.
  - Empty: no work in that milestone.
- **Blocked by** names what has to happen before that component's "done" cell can start: a decision waiting on the lead (listed under the component in the design doc) or a spike (a short throwaway test). A component with nothing blocking it can start when the lead says so.
- **Tasks** are GitHub issues, each in one milestone. Milestone 1's tasks are listed below. The other milestones get theirs before they start.

### How we track it (Claude's proposal)

A GitHub milestone shows the percentage of its issues that are closed. Under part-by-part milestones, that tells you how far along a part is. It doesn't do everything the lead asked for, so three additions:

1. **Feature labels.** Users see features, and a feature crosses parts. Parental controls, for example, touches the profiles service, the DNS filter, Firefox's website limits and the settings screens. Each issue gets a `feature:` label (`feature: parental-controls`). Filtering by it shows how complete that feature is across milestones.
2. **A "Done when" issue per milestone.** It holds the milestone's acceptance checks and is closed last. Every issue closed doesn't prove the box works; this issue does.
3. **A `blocked` label** on issues waiting for a decision or spike, naming it, so the board matches the grid.

Other labels: `kind:` screen, service, API, script, config, test, docs.

## Order of the parts (Claude's proposal)

| # | Milestone | Why here | You can try at the end |
|---|---|---|---|
| 0 | Ready to code | Every product-code task needs the repo layout, build system and checks first | Nothing new; a build and test run on an empty project |
| 1 | Shell | Everything with a screen needs its theme and controls; it runs in the dev box today, with no OS image | The home screen in a window: launch apps, Menu, volume, the on-screen keyboard, notifications |
| 2 | Web apps | The main thing people watch; needs the shell's key handling and menu | YouTube, Twitch and Dropout with the air mouse, in the dev box |
| 3 | OS image and releases | Its good-boot check, installer page and recovery screens need the shell | Install from USB on a VM and the OptiPlex, update, roll back |
| 4 | Profiles | Needs a real image to create users and run greetd | A family of profiles with PINs and a kid profile with limits |
| 5 | Hardware | Hardware check and add-ons need profiles' setup flow and the image | Codecs and drivers built on the box, gamepads and CEC, sleep and wake |
| 6 | Settings and setup | The pages drive services from parts 3–5 | Set up a new box and change everything from the sofa |
| 7 | Alpha | Release stage, not a part: everything together on real boxes | A week as the household TV box, no keyboard |
| 8 | 1.0 | Release stage: signing, docs, reviews, beta | Someone else installs it from the guide |

Cost of going part by part (raised 2026-11-22 when the lead chose it): Hearth first runs as a whole on a real box at milestone 3, and users can't use any feature until every part is done. Putting the shell first means there's something to try from milestone 1.

## The grid

M0 Ready to code · M1 Shell · M2 Web apps · M3 OS image · M4 Profiles · M5 Hardware · M6 Settings · M7 Alpha · M8 1.0

| Part | Component | M0 | M1 | M2 | M3 | M4 | M5 | M6 | M7 | M8 | Blocked by |
|---|---|---|---|---|---|---|---|---|---|---|---|
| OS image | Repo and code checks | done | | | | | | | | | Repo layout and tools (Claude proposes, lead OKs); hosted reviewers |
| Shell | Session setup | | done | | + started by greetd | + per-profile sessions | | | | | — |
| Shell | Theme and controls | | done | | | | | | | + accessibility pass | Text sizes; safe margins; motion; focus never follows pointer; variable fonts; contrast test; accessible names |
| Shell | Home screen | | done | + web apps | | + per-profile order | | | | | Arranging apps; per-profile order; still backgrounds; particle limits |
| Shell | Overlays | | done | | | + profile switcher | | | | | Screensaver delay; how to detect "no internet" (to research) |
| Shell | Key handling | | done | + bridge | | | + gamepads, CEC | + remapping | | | — |
| Shell | System menu | | done | + web-app items | | | | | | | MPRIS and borrowing |
| Shell | On-screen keyboard | | done (gamepad wheel: stub) | + `inputmode` | | | + gamepad wheel | | | | Keyboard build approach; layout per field; suggestion engine (Claude's pick); spike: keyboard beyond Firefox |
| Shell | Notifications | | done | | | | | | | | — |
| Shell | Failure screens | | done (design + shell's own) | + web apps' | + image's | + profiles' | + hardware's | + settings' | | | — |
| Web apps | Web-app definitions | | | done | | + allowed profiles | | + "add a web app" | | | Engine-neutral format; web-app channel; extensions from a URL |
| Web apps | Firefox setup | | | done | + in the image | + website limits | + hardware decode | | | + lighter Firefox | Firefox language; pop-ups off; spike: Firefox from outside; uBlock Origin (lighter-Firefox review, parked) |
| Web apps | `hearth-firefox-bridge` | | | done (unsigned, dev box) | + signed, installed by policy | | | | | | Bridge rules (plain JavaScript, actions only); the lead's addons.mozilla.org account |
| OS image | Image build | | | | done | | | | | | Block unneeded drivers; AdGuard source archive; AdGuard signature check |
| OS image | Boot and updates | | | | done | | | + Updates page | | | "Viewing" test; pin systemd per release; timers catch up; spike: updates and rollback |
| OS image | Installer | | | | done | | | | | | Spike: installer |
| OS image | Recovery and diagnostics | | | | done | | | | | | — |
| OS image | Network and permission rules | | | | done | + group rules | | | | | What listens; spike: record a real boot |
| OS image | CI | | | | done | | | | | | — |
| OS image | Publishing | | | | done (unsigned) | | | | | + signing | — |
| Profiles | Login screen | | | | stub (one user, logged in automatically) | done | | | | | Spike: profiles and logins |
| Profiles | Profiles service | | stub (placeholder name) | | stub (one user) | done | | | | | Plain `useradd`; spike: profiles and logins |
| Profiles | DNS filter | | | | stub (one client) | done | | | | | Spike: per-profile DNS; spike: AdGuard Home as a service |
| Profiles | Screen-time keeper | | | | | done | | | | | — |
| Profiles | Parental-control screens | | | | | done | | + inside Settings | | | Jellyfin users per profile; kid profile's power options |
| Hardware | Hardware check | | | | | | done | | | | Hardware tiers; forcing H.264 on legacy boxes |
| Hardware | Hardware add-ons | | | | | | done | | | | Add-on method; NVIDIA licence screen; where the compiler comes from; spike: codec add-on |
| Hardware | Gamepads and HDMI-CEC | | | | | | done | | | | — |
| Hardware | Power and sleep | | | | | | done | | | | Waking from each remote |
| Settings | Settings app | | | | | stub (frame for parental screens) | | done | | | TV colour range; HDR in 1.0 or after; TV-first sound rule |
| Settings | First-boot wizard | | | | | | | done | | | Moonlight and Moonfin: offered or on every box |
| Settings | Apps | | stub (launch only) | | | | | done | | | Spike: portals in a Hearth session |
| Docs | Help pages | | | | | | | | done | | — |
| Docs | Install guide | | | | | | | | | done | — |
| Docs | User and contributor docs | | + shell | + web apps | + build and release | | | | | done | Diátaxis layout |
| Phone | KDE Connect features | not placed | | | | | | | | | The phone in 1.0, or after |

## Milestones

### 0. Ready to code

Done when: the lead has OK'd the repo layout, build system, test framework and code checkers; an empty QML/C++ project builds and its checks run.

Tasks:
- Propose the product code layout, build system, test framework and code checkers (Claude), for the lead's OK.
- Decide the code-review setup: hosted reviewers, if any, and local checks.
- Branch protection for `main`. Free GitHub accounts don't offer it on private repos ([agentic review checks](research/2026-11-19-agentic-review-checks.md)), so either the repo goes public or the protection waits.
- Empty project: builds in the dev box, the checkers run, one test passes.

### 1. Shell

Done when: in the dev box, labwc in a window, with the air mouse: the home screen shows installed apps in rows; an app opens full screen and Home comes back; Menu opens the system menu over it; volume shows over it; the on-screen keyboard types into Firefox, a GTK4 app, a Qt app and Moonlight; a notification reaches the bell; a failed app launch shows a failure screen; every screen passes the high-contrast check; the headless test suite passes.

Tasks, by component:

**Session setup**
- Session start script: labwc, the shell and the keyboard, with the host desktop's settings stripped
- labwc key bindings: Back, Home, Menu, Esc, Backspace, fired on release
- Window rule that forces apps full screen

**Theme and controls**
- Theme token file and loader; the default dark theme (blocked: text sizes, safe margins, motion)
- High-contrast theme
- Fonts: Atkinson Hyperlegible Next and Noto Sans (blocked: variable fonts)
- Every string through KI18n
- Focus ring and focus rules (blocked: focus never follows the pointer)
- Controls: tile and grid
- Controls: button, toggle, slider
- Controls: list
- Controls: text box
- Controls: dialog
- Accessible name and role on every control (blocked: accessible names)
- Contrast check for every built-in theme (blocked: contrast test)

**Home screen**
- App list: desktop files and Flatpaks
- Rows of tiles with the next row peeking in; focus and scroll animation
- Launch full screen, Home brings the home screen forward, return to a running app
- Rows-and-columns value read from config (the setting's page comes in milestone 6)
- Background image (blocked: still backgrounds)
- Arrange apps (blocked: arranging apps)
- Celebrations and particles (blocked: particle limits)

**Overlays**
- Status overlay: clock, network state, placeholder profile name, bell, power (sleep, restart, off)
- Burn-in protection: dim and shift
- Screensaver (blocked: screensaver delay)
- Volume overlay through PipeWire
- "No internet" when something fails (blocked: research how to detect it)

**Key handling**
- Key router: what each key means now (home screen, overlay, text box, app)
- Held-key rules
- Default key map for air mice

**System menu**
- Menu overlay with its system section: go home, close app, power
- Menu Contribution API, version 1: format and limits, written as a spec
- D-Bus interface for native apps
- Play and pause for any media app through MPRIS (blocked: MPRIS and borrowing)

**On-screen keyboard** (all blocked: keyboard build approach)
- Keyboard frame: input-method v2, virtual-keyboard, layer-shell strip
- QWERTY grid layout
- Alphabet grid layout
- Single-row layout
- Gamepad wheel: stub, driven by arrows
- Layout per kind of field (blocked: layout per field)
- Word suggestions, kept per profile, with an off switch (Claude picks the engine)
- A real keyboard's letters pass through; held arrows repeat; key names through libxkbcommon
- Spike: types into GTK4, Qt and Moonlight Flatpaks

**Notifications**
- Notification service (freedesktop Notifications over D-Bus)
- Bell list and short pop-ups

**Failure screens**
- The failure-screen design and its control
- Error code list format, and the shell's own codes
- QR code for each code (needs the lead's OK for a QR library)

**Tests and docs**
- Headless test suite, grown from the spike's checks
- Shell contributor doc

### 2. Web apps

Done when: in the dev box, YouTube, Twitch and Dropout open from tiles, work end to end with the air mouse, Back behaves on each site, a site's menu items show in the system menu, and the keyboard stays closed on pages that bring their own.

Flag for the lead: this is where web-app integration starts to matter, and the lead asked to revisit Firefox against other engines then.

### 3. OS image and releases

Done when: a USB-installed box, in a VM and on the OptiPlex, boots into the shell, updates itself, rolls back a broken version and opens the recovery menu; CI builds the image and fails on any connection not on the allowlist. The shell's real-hardware checks run here: real screen, overlay over playing video, TV colour range.

### 4. Profiles

Done when: on an installed box, a family of profiles can be made, switched and deleted; PINs and the admin PIN work; a kid profile stays inside its app, website, DNS and screen-time limits.

### 5. Hardware

Done when: the hardware check sorts the test boxes correctly; codecs build on the box and survive an update; a failed add-on falls back with a notice; gamepads and a CEC remote drive the shell; sleep and wake work from each remote.

### 6. Settings and setup

Done when: a fresh box is set up with the first-boot wizard and only a remote, and every settings page works from the remote.

### 7. Alpha

Done when: a week in a real living room with no keyboard, no SSH and no lost viewing; the RAM budget holds on a 4 GB box; every error code has a help page; testers have a guide and a way to report problems.

### 8. 1.0

Done when: signing is on; a stranger installs it from the guide and uses it as their TV box; the security review, accessibility pass and lighter-Firefox review are done; beta brought fixes only.

## Not placed (needs the lead)

- **The phone:** in 1.0, or after? Typing from a phone doesn't work on labwc with KDE Connect as it stands (design doc, Phone).
