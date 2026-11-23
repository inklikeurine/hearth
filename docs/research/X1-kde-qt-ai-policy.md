# X1: KDE's policy on AI-assisted contributions

*Research by a Claude research agent, 2026-11-22, run at the lead's request; copied here as returned. Not re-checked line by line.*

**Answer:** None of the four bans AI-assisted work outright. KDE has wiki guidelines for the whole community. KDE Connect has its own, stricter rules. Plasma Bigscreen and Qt have nothing written down beyond "understand what you commit". KDE Connect is the only one that bans anything: it closes merge requests that are "fully or predominantly AI-engineered", and it closes any request whose description or review replies were written by AI.

| Project | Policy exists? | What it says (quote, under 40 words) | Link | Date checked |
|---|---|---|---|---|
| KDE-wide | Yes, as wiki guidelines (last edited 25 May 2026) [C] | "you will have to disclose the AI tools you have used in the merge request. In commit messages, do not include "Co-authored-by:" or "Assisted-by:" for these tools" [C] | [Maintainers and Contributions](https://community.kde.org/Guidelines_and_HOWTOs/Maintainers_and_Contributions) | 2026-11-22 |
| KDE-wide, bug reports | Yes [C] | "If you must use an LLM, synthesize its output into your own conclusion and then write it in your own words" [C] | [Issue Reporting](https://community.kde.org/Get_Involved/Issue_Reporting#Don't_paste_LLM_output) | 2026-11-22 |
| KDE Connect | Yes, added 3 Aug 2026 [C] | "does **not** accept merge requests that are fully or predominantly AI-engineered. AI tools may only be used to execute a design which has been planned by a human." **Partial ban** [C] | [AGENTS.md](https://invent.kde.org/network/kdeconnect-kde/-/blob/master/AGENTS.md), [CONTRIBUTING.md](https://invent.kde.org/network/kdeconnect-kde/-/blob/master/CONTRIBUTING.md) | 2026-11-22 |
| Plasma Bigscreen | None found | Looked in the repository's top-level files (no CONTRIBUTING or AGENTS file) and the README [C] | [plasma-bigscreen](https://invent.kde.org/plasma/plasma-bigscreen) | 2026-11-22 |
| Qt Project | None adopted | Only the general Commit Policy rule: "Do not commit anything you do not understand." [C] An AI proposal was drafted in July 2026 [C] but has not been adopted [C] | [Commit Policy](https://wiki.qt.io/Commit_Policy), [mailing-list post](https://lists.qt-project.org/pipermail/development/2026-July/047337.html), [proposal index](https://contribute.qt-project.org/quips/0), [proposal repository](https://code.qt.io/cgit/meta/quips.git/) | 2026-11-22 |

## Details that matter for the QR-pairing contribution

- **KDE Connect allows:** AI writing code "when the contributor has already designed the solution", provided you fully understand it, can debug it yourself, and can discuss it with reviewers without AI help. You must disclose AI use when it "meaningfully contributed" [C].
- **KDE Connect closes on sight:** AI-written merge-request descriptions or review replies, features you don't understand, and automated submissions (which "may result in contributor ban") [C].
- **The two rule sets clash on commit tags.** The KDE-wide page says not to add an "Assisted-by:" line. KDE Connect's AGENTS.md (instructions written for AI coding tools) says to add "Assisted-by: <assistant name>" when the AI commits [C]. The agent's recommendation: ask the KDE Connect maintainers which rule applies before the first merge request.
- **KDE-wide:** maintainers may close merge requests where someone has relied "on an LLM to generate a patch" without understanding it, and a tool cannot be the copyright holder [C].
- **A stricter KDE-wide policy stalled.** In September 2026 a KDE developer, Nate Graham, proposed stricter rules for the whole community. The discussion thread was removed and nothing was adopted [S: [The Register](https://www.theregister.com/software/2026/09/24/talk-of-ai-in-kde-sets-the-community-ablaze/5298854)].
- **Qt:** the AI proposal is a QUIP (Qt Improvement Proposal, Qt's formal process for project-wide rules), Gerrit change 742364 (Gerrit is Qt's code-review site) [C]. Its author favoured disclosing AI use without naming the vendor [C].

## Not confirmed

- The Qt proposal's text and current status. Gerrit refused to load it, so the agent only knows it has not been merged as of the proposal repository's last commit on 2026-10-21.
- Whether the Plasma Bigscreen maintainers follow an unwritten rule, or apply the KDE-wide page by default.
- Whether KDE Connect's rules cover the Android app. Its repository has no AGENTS or CONTRIBUTING file, though the desktop repository's AGENTS.md names kdeconnect-android.
- Whether the KDE-wide page binds anyone. It sits under "Guidelines and HOWTOs", not under "Policies".
- KDE e.V. announcements weren't checked in detail. The KDE manifesto says nothing on AI.
