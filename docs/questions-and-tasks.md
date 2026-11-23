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
- Apps: Jellyfin, Moonlight, and Firefox kiosk "web apps" (YouTube TV interface, Twitch, Dropout) with our own Firefox extension, hearth-firefox-bridge. One global key map for every app.
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
| Kernel lockdown default: off, on, or on except where an outside driver is needed (proposed)? Research is back: lockdown blocks unsigned drivers even with Secure Boot off, and can be raised after a driver loads | Lockdown would block drivers pulled at setup time | Design doc, section 5.6; [lockdown and module signing](research/D3-lockdown-module-signing.md) |
| NVIDIA floor: Pascal (GTX 10) or Turing (GTX 16 / RTX 20)? Research is back: NVIDIA's current driver supports Turing and newer; Pascal is on the older 580 branch, with security fixes promised through October 2028 | Which cards count as supported, and for how long | Design doc, section 5.6; [NVIDIA's supported GPUs](research/D4-nvidia-support-and-licence.md) |
| Boot-time target | Shapes the image and startup work | Design doc, section 3 |
| Casting in 1.0: AirPlay and DLNA receivers as opt-in switches, or later? | Each one is a listening port | Design doc, section 7 |
| Deferred until the architecture is settled: review hosted code-reviewer options (which, if any, and what runs locally instead) | They send our code off the machine; background in the design doc | Design doc, section 17 |
| Paid streaming switch: in 1.0 or later? | Requires Google's copy-protection download | Design doc, section 8.4 |
| Rating names and 12-month expiry for Tested with Hearth | Public-facing wording | [Tested with Hearth](tested-with-hearth.md) |
| Help-page hosting for error codes (codes are human readable: agreed) | Every failure screen's QR code points there | Design doc, section 15 |
| Saved logins: what happens when the keyring is lost, and a "Saved logins" settings page to forget or reset logins (to scope) | The failure screen and Settings both depend on it | Design doc, sections 10 and 15; [Secret store: oo7](decisions/0001-keyring-oo7.md) |
| The admin PIN: an admin profile's own PIN, or a separate box-wide PIN? Deferred (the lead, 2026-11-22). | Parental controls are a primary feature; the onboarding flow sets it up | [Profile PINs and the admin PIN](decisions/0017-profile-pins-and-admin-pin.md); design doc, section 13 |
| Feature review: make Firefox kiosk lighter. Which RAM and startup cuts are worth their security or compatibility cost? | One web app must fit the 4 GB RAM budget | Design doc, section 17 |
| Branch protection for `main`: GitHub refuses it on a private repo with a free account. Make the repo public, pay for GitHub Pro (about $4 a month), or rely on local hooks until it goes public? Later (the lead, 2026-11-22). Meanwhile local hooks refuse force pushes to `main` and Claude asks before every push ([dev environment](dev-environment.md)) | Without it, nothing on GitHub stops a bad push to `main` | [Agentic review checks](research/2026-11-19-agentic-review-checks.md) |
| Network control for apps the user installs: is Flatpak's network permission enough, or does Hearth need per-app control (a firewall rule per app, a "this app wants to connect" prompt)? For a design session, not Sonnet. Shapes the settings screens | Local-only promise without auditing every app | Design doc, section 11; [allowlist draft](research/2026-11-19-network-allowlist-first-draft.md) |
| Firefox studies (Mozilla's experiments, delivered through Remote Settings): off, or allowed with the rest of Firefox's connections? Telemetry stays off either way | Studies can change Firefox's behaviour on boxes without a Hearth release | Design doc, section 2 (network defaults) |
| Screen mirroring: when to revisit | Lowest-priority phone feature | Design doc, section 7 |
| Translation system for Hearth's own text: Qt's built-in `qsTr()`, or KDE's `i18n()` (KI18n) that Bigscreen uses? | Every string in the shell is written one way or the other from the first line of product code | Design doc, section 9.3; [Bigscreen translations](research/L1-bigscreen-translations.md) |
| QR-code pairing for KDE Connect: still a planned contribution? KDE Connect refuses merge requests "fully or predominantly AI-engineered" and expects the contributor to design it, understand it and discuss it without AI | The only planned upstream contribution | Design doc, sections 12.3 and 16; [KDE and Qt AI policy](research/X1-kde-qt-ai-policy.md) |
| On-screen keyboard details: word suggestions are agreed; kept per profile? an off switch? | Typing is the worst part of a TV | Design doc, sections 2, 6.2 and 6.4; [shell spike results](research/2026-11-22-shell-spike-results.md) |

## Research tasks for a Sonnet chat

None waiting. The eleven prompts that were here were run by Claude research agents on 2026-11-22 (the lead's request) and folded into the design doc: [Bigscreen translations](research/L1-bigscreen-translations.md), [font sizes per script](research/L2-font-sizes-per-script.md), [system locales](research/L3-system-locales.md), [how Firefox picks a language](research/L4-firefox-language.md), [Firefox memory settings](research/F4-firefox-memory-prefs.md), [TV platforms' web-app features](research/G1-tv-web-app-apis.md), [drivers as system extensions](research/D2-driver-sysext.md), [lockdown and module signing](research/D3-lockdown-module-signing.md), [NVIDIA's supported GPUs and licence](research/D4-nvidia-support-and-licence.md), [KDE Connect's October 2026 advisories](research/C3-kdeconnect-oct-2026-advisories.md), [KDE and Qt AI policy](research/X1-kde-qt-ai-policy.md). New prompts go here, written for a fresh chat with the context block above.

## Spikes to add

Short hands-on tests found in design sessions. The labwc spike's software checks are done (design doc, section 5.4); its hardware checks wait for the software work.

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
| Get a USB infrared receiver (the WeChip air mouse is already in hand) | Default key-map testing | About $15 |
| When the bridge is ready to sign: an addons.mozilla.org developer account and API keys | Unlisted signing needs them | 15 min |
| Paste the summary prompt (above) into the threat-model chat and bring the reply back | It's the only earlier chat not yet in the repo | 10 min |
| Decide the open questions above, a few at a time | Unblocks decision records | Ongoing |
