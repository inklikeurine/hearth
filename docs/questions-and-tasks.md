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
- Hardware floor: 4 GB RAM, x86-64, graphics with VP9 hardware decode: Intel 7th gen or newer, AMD Raven Ridge-class APUs (2018) or newer, NVIDIA GTX 16 / RTX 20 or newer (GTX 10 is legacy until October 2028). Raspberry Pi 4 is a design goal only.
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
| The roadmap's part order, grid, labels and milestone 1 tasks (Claude's proposal, 2026-11-23) | Gives development an order and a definition of done for each piece | [Roadmap](roadmap.md) |
| Product code: does "the grid shows which open decision blocks which cell" mean a piece can start once nothing blocks it, when the lead asks? (Claude's reading) | Decides when code can start | Design doc, section 2 |
| The "Waiting on the lead" lines under each component | Each one blocks a cell in the roadmap grid; milestone 0 and 1's come first | [Design doc](design.md) |
| Boot-time target. Later (the lead) | Shapes the image and startup work | Design doc, section 2 |

## Research tasks for a Sonnet chat

None waiting. The eleven prompts that were here were run by Claude research agents on 2026-11-22 (the lead's request) and folded into the design doc: [Bigscreen translations](research/L1-bigscreen-translations.md), [font sizes per script](research/L2-font-sizes-per-script.md), [system locales](research/L3-system-locales.md), [how Firefox picks a language](research/L4-firefox-language.md), [Firefox memory settings](research/F4-firefox-memory-prefs.md), [TV platforms' web-app features](research/G1-tv-web-app-apis.md), [drivers as system extensions](research/D2-driver-sysext.md), [lockdown and module signing](research/D3-lockdown-module-signing.md), [NVIDIA's supported GPUs and licence](research/D4-nvidia-support-and-licence.md), [KDE Connect's October 2026 advisories](research/C3-kdeconnect-oct-2026-advisories.md), [KDE and Qt AI policy](research/X1-kde-qt-ai-policy.md). New prompts go here, written for a fresh chat with the context block above.

## Spikes to add

Short hands-on tests found in design sessions. The labwc spike's software checks are done ([design doc detail](research/2026-11-23-design-doc-detail.md), section 5.4); its hardware checks wait for the software work.

- Per-user DNS through the firewall: can nftables make each Unix user's lookups reach AdGuard Home from a different local address, so it sees separate clients? Fallback: one AdGuard Home per profile ([AdGuard per-profile check](research/2026-11-19-adguard-per-profile.md)).
- Firefox shaped from outside: unsandboxed AutoConfig works in the Firefox build the image ships; which built-in keys it can rebind (Escape, Backspace, Back); what kiosk mode does to context menus and dialogs ([Firefox outside customising](research/2026-11-19-firefox-outside-customising.md)).
- Profiles and sessions, in a test image ([how profiles run](research/2026-11-23-design-doc-detail.md), section 13.2): does Fedora's greetd login config register sessions with logind; what is Fedora's `KillUserProcesses=` default (`systemd-analyze cat-config systemd/logind.conf`); do Firefox and a Flatpak app stop after `loginctl terminate-user` (`systemd-cgls`); do `useradd` and `userdel` work with `/usr` read-only.
- AdGuard Home as a service (design doc, DNS filter): AdGuard's own build runs as an unprivileged user with only the port-53 capability, next to systemd-resolved, with `--no-check-update`; record every connection it makes.
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
| Answer the "Waiting on the lead" lines in the [design doc](design.md): agree / disagree / discuss | Turns proposals into decisions and unblocks the grid | 1 hour |
| Create the roadmap's milestones and labels on GitHub, once the roadmap is agreed (Claude has no tool for either in this session) | Then Claude files each milestone's tasks as issues | 15 min |
| Get a real potato test box: an old 4 GB laptop or mini-PC with 7th-gen Intel or a Raven Ridge AMD APU. | The floor needs a test machine; the OptiPlex is above it | Cheap or free |
| Get a USB infrared receiver (the WeChip air mouse is already in hand) | Default key-map testing | About $15 |
| When the bridge is ready to sign: an addons.mozilla.org developer account and API keys | Unlisted signing needs them | 15 min |
| Paste the summary prompt (above) into the threat-model chat and bring the reply back | It's the only earlier chat not yet in the repo | 10 min |
| Decide the open questions above, a few at a time | Unblocks decision records | Ongoing |
