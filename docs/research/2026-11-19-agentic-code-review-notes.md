# Agentic code review: session notes (2026-11-19)

## 1. Decisions
None. Three questions were asked (current practice, code quality, when/how to add it). No choices made.

## 2. Found, not confirmed
All from search snippets (secondary sources). No page opened in full.
- AI tools invent package names and attackers register them. Lockfiles plus human or allowlist approval for new packages: [Cloud Security Alliance note](https://labs.cloudsecurityalliance.org/research/csa-research-note-slopsquatting-ai-supply-chain-20260419/)
- Duplication up 81%, reuse down 70% in AI-era commits (GitClear, via press): [LeadDev](https://leaddev.com/ai/code-maintainability-plummets-in-the-ai-coding-era)
- Example gate for new code: no new issues, 80% coverage, 3% or less duplication (vendor suggestion): [SonarSource](https://www.sonarsource.com/ko/blog/how-to-optimize-sonarqube-for-reviewing-ai-generated-code/)
- Rules files are advice; hooks, CI and branch protection bind (blog claim): [Axon Build](https://axonbuild.com/blog/guardrails-for-ai-coding-agents/)

## 3. Unsure or unchecked
- No official docs checked (Claude Code settings, AGENTS.md, branch protection). The deny-rule snippet I gave is untested.
- Figures like "AI tests catch ~4% of bugs", mutation scores of 40-55%, "45% of AI code is vulnerable" come from vendor blogs. Unverified.
- Two sources disagree on when Claude Code reads AGENTS.md.
- Tools I named (Ruff, ESLint, Stryker, knip, Sheriff) are Python/JS/Angular tools. I never researched QML/C++ tooling. qmllint, Qt Quick Test and clang-tidy are my guesses, unchecked.
- The advice was written before I knew Hearth's stack.

## 4. Open questions
- Where is the repo hosted? Asked twice, unanswered.
- Does KDE Plasma Bigscreen (or Qt) have an AI policy? Not checked. Will Hearth send changes upstream?
- Which agent and reviewer models? Are hosted cloud reviewers acceptable for development?
- How are image packages pinned in mkosi builds (e.g. a Fedora snapshot)?

## 5. Conflicts with Hearth rules
- Hosted reviewers/scanners (Copilot review, Qodo, SonarQube Cloud, GitGuardian) send code off the machine. Local-only covers the product, not dev tools, but decide. Offline options: Semgrep CLI, gitleaks.
- Playwright installs Chromium by default, against the no-Chromium rule in spirit. Firefox only, and it can't test the QML UI.
- Lockfile advice is npm-shaped. It doesn't cover Fedora packages in mkosi or Flatpak refs.
- A weekly cleanup agent on forked Bigscreen QML will cause upstream merge conflicts (my inference).
- No conflict found: systemd-sysupdate, 4 GB floor (tools run on dev/CI, not in the image), KDE Connect, AdGuard Home, Firefox web apps.
