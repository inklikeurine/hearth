# The lead's questions and tasks

*Running log. It holds only what's still open. When a question is answered or a task is finished, its line is deleted: the answer goes into the [design doc](design.md), research results into `docs/research/`, and locked-in decisions into decision records.*

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
- Hardware floor: 4 GB RAM, x86-64: Intel 7th gen or newer, AMD Raven Ridge-class APUs (2018) or newer, NVIDIA GTX 16 / RTX 20 or newer (GTX 10 is legacy until October 2028). Raspberry Pi 4 is a design goal only.
- Optional hardware: on-board Wi-Fi, Bluetooth, USB infrared receivers, gamepads and HDMI-CEC are recommended and supported by default, but never assumed present; old laptops, desktops and mini-PCs often lack them.
- Platform: Hearth's own Qt 6 QML shell (GPL-3.0-or-later), borrowing parts from KDE Plasma Bigscreen (no Plasma underneath). One full-screen app at a time; Hearth draws the overlays and the Menu-button system menu, and apps add menu items through a Hearth API. Compositor: labwc for 1.0.0; niri replaces it before 2.0.0. Heavy accessibility features (screen reader, color filters) come in 2.0.0. OS image built with mkosi on Fedora 45, installed from an "Installer" boot profile running systemd-sysinstall, updated with systemd-sysupdate (A/B, read-only system, automatic rollback). Boots on UEFI only. Fedora's stock kernel, Secure Boot off by default in 1.0; drivers Fedora doesn't ship (NVIDIA, some Wi-Fi) are pulled at setup time for the hardware found.
- Hearth's own parts live in the OS image, never as Flatpaks; any other app can be a Flatpak.
- Each profile is its own Unix user (one active session at a time, optional PIN); logins are kept in oo7, the secret store.
- Apps: native apps and Firefox kiosk "web apps", with our own Firefox extension, hearth-web-bridge. Which apps ship isn't decided. One global key map for every app.
- Phone features come in 2.0.0. Ad and tracker blocking on the box: AdGuard Home, pulled from AdGuard's own releases and pinned by version and checksum.
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

## Open questions

| Question | Why it matters | Where it's discussed |
|---|---|---|
| The roadmap's part order, grid, labels and milestone 1 tasks (Claude's proposal, 2026-11-23) | Gives development an order and a definition of done for each piece | [Roadmap](roadmap.md) |
| The "Waiting on the lead" lines under each component | Each one blocks a cell in the roadmap grid; milestone 0 and 1's come first | [Design doc](design.md) |
| Boot-time target. Later (the lead) | Shapes the image and startup work | Design doc, section 2 |
| App tile art: where wide art comes from, the default tile, whether art is ever fetched online. Later (the lead) | Most Linux apps ship only square icons | Design doc, Home screen; [visual references](research/V1-visual-references.md) |
| Guest profile: the rest of the details. Later (the lead) | First rules are agreed; more thought needed | Design doc, Guest profile |
| Gamepad-wheel keyboard: needs its own design time. Later (the lead) | Round 2 mockup didn't work | Design doc, On-screen keyboard |
| Volume overlay design. UI review round 3 (the lead) | Both round 2 versions rejected; right edge is a stand-in | [Visual references](research/V1-visual-references.md) |
| Notifications' look, and the menu in more detail. Future UI rounds (the lead) | Neither has been mocked up | [UI guide](ui-guide.md) |
| Profile picture crop controls: mock-up and flow. Later (the lead) | Needed before import can be built | Design doc, Profile pictures |
| Is importing profile pictures admin-only? To consider (the lead) | Pictures are shared by every profile | Design doc, Profile pictures |
| Review the failure screen's QR-code privacy wording. At alpha and each release candidate (the lead) | Depends on what diagnostic data the code carries | Design doc, Failure screens |

## Research tasks for a Sonnet chat

New prompts go here, written for a fresh chat with the context block above.

### Keyboard: its own copy of the text field (2026-11-26)

```
Hearth's on-screen keyboard talks to labwc through input-method-unstable-v2; apps use text-input-unstable-v3. The keyboard panel can cover the app's own text field, and the lead wants the text being typed shown inside the keyboard panel.

Research, with primary sources (protocol XML, toolkit source, official platform docs), tagging each claim [C] [S] [M]:
1. Which messages in text-input v3 and input-method v2 carry the field's current text (surrounding text), the cursor's screen position (cursor rectangle) and the field type (password, number, email)? Link the XML.
2. Do Firefox on Wayland (GTK3 input method), GTK4, Qt 6 and SDL3 (Moonlight) send surrounding text, cursor rectangle and content type? Link source lines; say what's missing.
3. Does labwc (or wlroots) pass surrounding text and cursor rectangle from text-input v3 to the input-method v2 client? Link source.
4. Do tvOS, Android TV (Gboard for TV) and Fire TV keyboards show their own copy of the field? Official docs only.
5. Options for Hearth (always copy / copy only when covered / move the keyboard away from the field / none) and what each needs from 1-3. Passwords: dots, or nothing?

Out of scope: layouts, word suggestions, X11. Save a summary: facts confirmed, unconfirmed, open questions, conflicts with labwc, local-only, no Chromium/Electron. Under 500 words.
```

## Spikes to add

The software spikes ran in a VM on 2026-11-24 ([test image results](research/2026-11-24-test-image-results.md)). Left for a VM: signed updates, SELinux enforcing, rebuilding the codec add-on after an update. Left for real hardware: labwc's Vulkan renderer and HDR, video decode, TV colour range, sound, sleep and wake, RAM on a 4 GB box, overlays over video, driver add-ons.

For 2.0.0: the niri spike, in the [roadmap](roadmap.md)'s 2.0.0 milestone ([niri before 2.0.0](decisions/0020-niri-before-2-0.md)).

## Summarizing a chat for the design log

Paste this into any Hearth chat before closing it, and save the reply to the Hearth project on claude.ai. Bring it to a design session, where it's copied into `docs/research/`.

```
Summarize this conversation for Hearth's design log, so a fresh session can use it without the chat:
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
