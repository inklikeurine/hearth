# E5 — Compositor upkeep and TV/kiosk use (KWin, labwc)
 
Checked 2026-11-20. Tags: [C] read in primary source, [S] secondary, [M] memory.
 
## 1. Maintenance
 
| | KWin | labwc |
|---|---|---|
| Maintainer | KDE project. No maintainers file in the repo (NOT FOUND). Most commits in the last 12 months: Xaver Hugl 618, Vlad Zahorodnii 590 [C] | Lead maintainer Johan Malm [C: [README](https://github.com/labwc/labwc/blob/aabfaf1/README.md)]. Most commits: Malm 79, tokyo4j 56, John Lindgren 53, Consolatis 45 [C] |
| Latest release | [v6.8.3](https://github.com/KDE/kwin/tree/v6.8.3), 2026-11-10 (tag date) [C] | [0.20.3](https://github.com/labwc/labwc/releases/tag/0.20.3), 2026-10-30 [C] |
| Contributors, 2025-11-20 to 2026-11-20 | 158 distinct commit authors, excluding the translation bot; 2,218 commits on master [C] | 50 distinct commit authors, excluding Weblate; 355 commits; 11 with 5+ commits [C] |
 
The counts come from my own count over the git history (author names, so one person under two names counts twice). Neither project's own statistics page was read.
labwc's README says it has been conservative with new features since early 2026, and calls itself not dormant [C].
 
## 2. TV or kiosk shells on these compositors
- **Plasma Bigscreen runs on KWin, but with plasmashell**: its README starts `kwin_wayland "plasmashell -p org.kde.plasma.bigscreen"` [C: [repo](https://github.com/KDE/plasma-bigscreen)]. KDE's own docs say not yet a daily driver [S: [docs](https://plasma-bigscreen.org/docs/introduction)]. Revived in July 2025 by one volunteer, who has since returned to Plasma Mobile [S: [Linux Magazine](https://www.linux-magazine.com/Online/News/Plasma-Bigscreen-Returns)].
- Bigscreen's QML on KWin without plasmashell, or on labwc: NOT FOUND.
- Kiosk on labwc: a Raspberry Pi guide autostarts a full-screen Chromium [S: [guide](https://linuxjunkies.org/guides/raspberry-pi-kiosk-mode)]. Selkies Desktop uses labwc as a tiny shell for single-app containers [S: [docs](https://docs.linuxserver.io/selkies/components/selkies-desktop/)]. Neither is a TV interface.
- Similar TV shell on a different compositor: Couch, a GTK4 Jellyfin 10-foot UI run under Cage (a single-app kiosk compositor) [S: [repo](https://codeberg.org/robland/couch)].
## Flag: AI policies (ground rule)
- labwc: "We do not accept contributions containing AI-generated content", including issues and PR descriptions [C: [CONTRIBUTING.md](https://github.com/labwc/labwc/blob/aabfaf1/CONTRIBUTING.md)].
- KDE: no adopted policy as of 23 Sep 2026; draft guidelines are contested [S: [report](https://www.linuxcompatible.org/story/kdes-dont-be-lazy-ai-draft-sparks-ban-call/)]. Re-check before any KWin or Bigscreen contribution.
 
