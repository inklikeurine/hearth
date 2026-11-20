# Agentic review: what's already answered (2026-11-19)

The four "agentic review" prompts (H1–H4) checked in a design session against the repo itself, GitHub and mkosi's docs, before handing anything to Sonnet. Background: [agentic review notes](2026-11-19-agentic-code-review-notes.md). Tags: [C] checked today in the repo, on GitHub or in the project's own docs; [M] from memory, not checked.

## H1. Limits on Claude Code in this repo

- `CLAUDE.md` is one line that loads `AGENTS.md`, so Claude Code reads the working rules at the start of every session [C].
- Not set up yet, all cheap to add: an allow-list of read-only commands in `.claude/settings.json`; a hook that blocks force pushes, `git reset --hard`, `git clean -f` and `git checkout -- .`; a rule that stops reading `.env` or other secret files [M].

## H2. Protecting `main` on GitHub: not possible on the current plan

The repo is private on a free account. GitHub's API refuses both branch protection and rulesets for it: "Upgrade to GitHub Pro or make this repository public to enable this feature" [C, `gh api repos/inklikeurine/hearth/rulesets` and `.../branches/main/protection`, 2026-11-19]. The lead task "protect `main`" can't be done until the repo is public or the account is on Pro. Until then, local hooks (H1) would be the only guard.

## H3. Code checkers for Qt, QML and C++: wait for code

No product code exists. Choose and wire up checkers (qmllint, clang-tidy and the rest) when there's code and CI is set up. Hosted reviewers are already deferred until the architecture is settled (design doc, section 17).

## H4. Pinning what goes into the image: answered for mkosi

- mkosi's `Snapshot=` builds from a fixed snapshot instead of the latest packages, and supports Fedora through `https://kojipkgs.fedoraproject.org`. The `latest-snapshot` verb prints the newest snapshot ID, so a bump is a one-line change the lead approves [C, [mkosi man page](https://github.com/systemd/mkosi/blob/main/mkosi/resources/man/mkosi.1.md)]. `ToolsTreeSnapshot=` does the same for the build tools.
- Flatpak apps can be pinned to a commit (`flatpak update --commit=…`) and held back (`flatpak mask`) [M]. Only matters for Flatpaks Hearth lists; Hearth's own parts are never Flatpaks. Check when that list exists.
