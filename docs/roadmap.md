# Roadmap to 1.0.0 (proposed)

Milestones are GitHub milestones, used to track how complete a feature is and whether it's ready to ship; milestones go part by part; every milestone gets broken into tasks; the grid shows which open decision blocks which piece of work. The order of the parts, the grid and the tasks are Claude's proposal. Nothing here is decided except those answers.

## How it works

- **Parts and components** come from the [design doc's component map](design.md#3-component-map). Each part is one milestone. A milestone is done when every component in its part reaches "done" and its "Done when" checks pass.
- **Grid cells** say how far a component gets in each milestone:
  - **done**: finished for 1.0.0, including its failure screens.
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

## Order of the parts (Claude's proposal; web apps near the end is the lead's)

| # | Milestone | Why here | You can try at the end |
|---|---|---|---|
| 0 | Ready to code | Every product-code task needs the repo layout, build system and checks first | Nothing new; a build and test run on an empty project |
| 1 | Shell | Everything with a screen needs its theme and controls; it runs in the dev box today, with no OS image | The home screen in a window: launch apps, Menu, volume, the on-screen keyboard, notifications |
| 2 | OS image and releases | Its good-boot check, installer page and recovery screens need the shell | Install from USB on a VM and the OptiPlex, update, roll back |
| 3 | Profiles | Needs a real image to create users and run greetd | A family of profiles with PINs and a kid profile with limits |
| 4 | Hardware | Hardware check and add-ons need profiles' setup flow and the image | Codecs and drivers built on the box, gamepads and CEC, sleep and wake |
| 5 | Settings and setup | First-boot wizard, app management and the pages left over; the other pages ship with their features | Set up a new box and change everything from the sofa |
| 6 | Web apps | After the shell and most of the UI are settled (the lead, 2026-11-24) | Web apps on a real box, with a remote |
| 7 | Alpha | Release stage, not a part: everything together on real boxes | A week as the household TV box, no keyboard |
| 8 | 1.0.0 | Release stage: signing, docs, reviews, beta | Someone else installs it from the guide |

Cost of going part by part (raised 2026-11-22 when the lead chose it): Hearth first runs as a whole on a real box at milestone 2, and users can't use any feature until every part is done. Putting the shell first means there's something to try from milestone 1.

## The grid

M0 Ready to code · M1 Shell · M2 OS image · M3 Profiles · M4 Hardware · M5 Settings · M6 Web apps · M7 Alpha · M8 1.0.0

| Part | Component | M0 | M1 | M2 | M3 | M4 | M5 | M6 | M7 | M8 | Blocked by |
|---|---|---|---|---|---|---|---|---|---|---|---|
| OS image | Repo and code checks | done (empty project builds, its checks run) | | | | | | | | | Hosted reviewers |
| Shell | Session setup | | done | + started by greetd | + per-profile sessions | | | | | | — |
| Shell | Theme and controls | | done, with Appearance | | | | | | | + accessibility pass | — |
| Shell | Home screen | | done | | + per-profile order | | | + web apps | | | — |
| Shell | Overlays | | done | | + profile switcher | | | | | | — |
| Shell | Key handling | | done, with remapping | | | + gamepads, CEC | | + bridge | | | — |
| Shell | System menu | | done | | | | | + web-app items | | | — |
| Shell | On-screen keyboard | | done (gamepad wheel: stub) | | | + gamepad wheel | | + `inputmode` | | | — |
| Shell | Notifications | | done | | | | | | | | — |
| Shell | Failure screens | | done (design + shell's own) | + image's | + profiles' | + hardware's | + settings' | + web apps' | | | — |
| Web apps | Web-app definitions | | | | | | | done, with allowed profiles and "add a web app" | | | — |
| Web apps | Firefox setup | | | stub (in the image) | | | | done, with website limits and hardware decode | | + lighter Firefox | uBlock Origin (lighter-Firefox review, parked) |
| Web apps | `hearth-web-bridge` | | | | | | | done (signed, installed by policy) | | | Bridge rules (plain JavaScript, actions only); the lead's addons.mozilla.org account |
| OS image | Image build | | | done | | | | | | | — |
| OS image | Boot and updates | | | done, with the Updates page | | | | | | | — |
| OS image | Installer | | | done | | | | | | | — |
| OS image | Recovery and diagnostics | | | done | | | | | | | — |
| OS image | Network and permission rules | | | done | + group rules | | | | | | What listens |
| OS image | CI | | | done | | | | | | | — |
| OS image | Publishing | | | done (unsigned) | | | | | | + signing | — |
| Profiles | Login screen | | | stub (one user, logged in automatically) | done | | | | | | — |
| Profiles | Profiles service | | stub (placeholder name) | stub (one user) | done | | | | | | — |
| Profiles | DNS filter | | | stub (one client) | done | | | | | | — |
| Profiles | Screen-time keeper | | | | done | | | | | | — |
| Profiles | Parental-control screens | | | | done, inside Settings | | | | | | — |
| Hardware | Hardware check | | | | | done | | | | | — |
| Hardware | Hardware add-ons | | | | | done | | | | | Where the compiler comes from; spike: driver add-ons (needs hardware) |
| Hardware | Gamepads and HDMI-CEC | | | | | done | | | | | — |
| Hardware | Power and sleep | | | | | done | | | | | — |
| Settings | Settings app | | frame; Appearance, Accessibility, Remotes | + Updates, Network, Time, Reset, Developer mode | + Profiles | + Display, Sound, Bluetooth, Power | done | | | | TV colour range |
| Settings | First-boot wizard | | | | | | done | | | | — |
| Settings | Apps | | stub (launch only) | | | | done | | | | — |
| Docs | Help pages | | | | | | | | done | | — |
| Docs | Install guide | | | | | | | | | done | — |
| Docs | User and contributor docs | | + shell | + build and release | | | | + web apps | | done | — |
| Phone | Phone features | 2.0.0 | | | | | | | | | — |

## Milestones

### 0. Ready to code

Done when: the lead has OK'd the repo layout, build system, test framework and code checkers; an empty QML/C++ project builds and its checks run.

Tasks:
- Decide the code-review setup: hosted reviewers, if any, and local checks.
- Branch protection for `main`. Free GitHub accounts don't offer it on private repos ([agentic review checks](research/2026-11-19-agentic-review-checks.md)), so either the repo goes public or the protection waits.

### 1. Shell

Done when: in the dev box, labwc in a window, with the air mouse: the home screen shows installed apps in rows; an app opens full screen and Home comes back; Menu opens the system menu over it; volume shows over it; the on-screen keyboard types into Firefox, a GTK4 app, a Qt app and an SDL app; a notification reaches the bell; a failed app launch shows a failure screen; the Appearance, Accessibility and Remotes pages work; every screen passes the high-contrast check; the headless test suite passes.

Tasks, by component:

**Session setup**
- Session start script: labwc, the shell and the keyboard, with the host desktop's settings stripped
- labwc key bindings: Back, Home, Menu, Esc, Backspace, fired on release
- Window rule that forces apps full screen

**Theme and controls** (done: theme tokens and loader, dark and high-contrast themes with their contrast check, fonts, focus ring, tile, tile grid, button)
- Every string through KI18n
- Controls: toggle, slider
- Controls: list
- Controls: text box
- Controls: dialog
- Accessible name and role on every new control

**Home screen**
- App list: desktop files and Flatpaks
- Rows of tiles with the next row peeking in; focus and scroll animation
- Launch full screen, Home brings the home screen forward, return to a running app
- Rows-and-columns value read from config (its setting is on the Appearance page)
- Background image
- Arrange apps
- Celebrations and particles

**Overlays**
- Status overlay: clock, network state, placeholder profile name, bell, power (sleep, restart, off)
- Burn-in protection: dim and shift
- Screensaver (2-minute default, user settable)
- Volume overlay through PipeWire
- "No internet" when something fails (AdGuard answers "server failure")

**Key handling**
- Key router: what each key means now (home screen, overlay, text box, app)
- Held-key rules
- Default key map for air mice

**System menu**
- Menu overlay with its system section: go home, close app, power
- Menu Contribution API, version 1: format and limits, written as a spec
- D-Bus interface for native apps
- Play and pause for any media app through MPRIS

**On-screen keyboard**
- Keyboard frame: input-method v2, virtual-keyboard, layer-shell strip
- QWERTY grid layout
- Alphabet grid layout
- Single-row layout
- Gamepad wheel: stub, driven by arrows
- Layout per kind of field
- Word suggestions, kept per profile, with an off switch
- A real keyboard's letters pass through; held arrows repeat; key names through libxkbcommon

**Settings**
- Settings frame: the page list, opening a page, Back
- Appearance, Accessibility and Remotes pages

**Notifications**
- Notification service (freedesktop Notifications over D-Bus)
- Bell list and short pop-ups

**Failure screens**
- The failure-screen design and its control
- Error code list format, and the shell's own codes
- QR code for each code (qrencode)

**Tests and docs**
- Headless test suite, grown from the spike's checks
- Shell contributor doc

### 2. OS image and releases

Done when: a USB-installed box, in a VM and on the OptiPlex, boots into the shell, updates itself, rolls back a broken version and opens the recovery menu; CI builds the image. The shell's real-hardware checks run here: real screen, overlay over playing video, TV colour range.

### 3. Profiles

Done when: on an installed box, a family of profiles can be made, switched and deleted; PINs and the admin PIN work; a kid profile stays inside its app, DNS and screen-time limits.

### 4. Hardware

Done when: the hardware check sorts the test boxes correctly; H.264 is set up on every box and survives an update; a failed add-on falls back with a notice; gamepads and a CEC remote drive the shell; sleep and wake work.

### 5. Settings and setup

Done when: a fresh box is set up with the first-boot wizard and only a remote, and every settings page works from the remote.

### 6. Web apps

Done when: web apps open from tiles on an installed box and work end to end with a remote, Back behaves on each site, a site's menu items show in the system menu, the keyboard stays closed on pages that bring their own, and a kid profile stays inside its website limits.

Flag for the lead: this is where web-app integration starts to matter, and the lead asked to revisit Firefox against other engines then.

### 7. Alpha

Done when: a week in a real living room with no keyboard, no SSH and no lost viewing; the RAM budget holds on a 4 GB box; every error code leads to a help page; testers have a guide and a way to report problems.

### 8. 1.0.0

Done when: signing is on; a stranger installs it from the guide and uses it as their TV box; the security review, accessibility pass and lighter-Firefox review are done; beta brought fixes only; the shell's text sizes, margins, motion and keyboard have been tested on a TV.

