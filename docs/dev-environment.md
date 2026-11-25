# Development environment

*Set up 2026-11-22. For now, Hearth is developed on the lead's own machine, inside a Fedora 45 container. This page says how to use it and what guards the repo.*

## The box: `hearth-f45`

A [distrobox](https://distrobox.it/) is a container that feels like a second Linux install: it shares your user, your home folder, your screen and the network, but has its own system and packages. Hearth's is a clean Fedora 45, the same base the OS image uses, with only the packages listed in [tools/dev/distrobox.ini](../tools/dev/distrobox.ini).

| Task | Command (on the host) |
|---|---|
| Create it | `distrobox assemble create --file tools/dev/distrobox.ini` |
| Enter it | `distrobox enter hearth-f45` |
| Run one command in it | `distrobox enter hearth-f45 -- labwc --version` |
| Rebuild it from scratch | `distrobox assemble create --replace --file tools/dev/distrobox.ini` |
| Remove it | `distrobox assemble rm --file tools/dev/distrobox.ini` |

- **Its own home folder:** `~/.local/share/distrobox-homes/hearth-f45`. Firefox profiles and config files made inside the box land there, not in your real home. Your real home is still mounted, so the repo is at the same path inside and out.
- **Process limit:** the host's podman (4.9) can't start a box with distrobox's default "no process limit", so every program in it fails to start. The manifest sets a fixed limit of 8192 instead.

### What's in it (versions on 2026-11-22)

| Group | Packages | Version |
|---|---|---|
| Compositor under test | labwc, plus wlrctl (switch windows from the command line), wlr-randr, wayland-info, wtype, wev, foot (a terminal) | labwc 0.20.2 |
| Shell toolkit | Qt 6 (base, QML, Wayland, SVG, shader tools, Qt tools), layer-shell-qt (turns a Qt window into an overlay), wlr-protocols-devel (the window-switching protocol's definition) | Qt 6.11.2, layer-shell-qt 6.8.3 |
| Building | cmake, ninja, gcc-c++, pkgconf, git | cmake 4.3.0 |
| Checkers | qmllint and qmlformat (from Qt), clang-tidy and clang-format (clang-tools-extra), gitleaks (secret scanner) | clang-tools-extra 23.1.5, gitleaks 8.30.1 |
| Apps the spike tests against | Firefox, Stevia (a ready-made on-screen keyboard; it doesn't run on labwc, so the spike has its own test keyboard) | Firefox 160.0, Stevia 0.57.0 |
| Building OS images | mkosi (approved 2026-11-22; pinned to Fedora 45's build). Installing it prints a harmless grub warning: grub can't find a real disk inside a container | mkosi 26 |
| Product code (approved 2026-11-24) | kf6-ki18n-devel, libxkbcommon-devel, qrencode-devel, hunspell-devel, hunspell-en-US | KI18n 6.32.0, libxkbcommon 1.13.1, qrencode 4.1.1, hunspell 1.7.6 |
| Hearth's fonts | atkinson-hyperlegible-next-fonts, google-noto-sans-vf-fonts | 2.100, 20260601 |
| Basics the container image lacks | Mesa graphics drivers, Fedora's core sans fonts | |

Not in the box yet: Xwayland (the layer for old X11 apps; Firefox runs natively on Wayland).

### Running labwc

- **Headless, no window** (confirmed working, used for the [protocol check](research/2026-11-22-labwc-protocols-fedora45.md)):

  ```sh
  WLR_BACKENDS=headless WLR_RENDERER=pixman WLR_LIBINPUT_NO_DEVICES=1 labwc -S wayland-info
  ```

- **In a window on your desktop:** `spikes/shell/run.sh` (on the `spikes` branch) builds the shell spike and opens labwc as a window, with the home screen inside. Works (the lead, 2026-11-22). labwc opens as an X11 window because `DISPLAY` is set and no Wayland session is.

Borrowed files approved for the spike: two Wayland protocol definitions from Stevia v0.57.0 (input-method v2, virtual-keyboard v1), pinned by checksum in `spikes/shell/protocol/README.md` (2026-11-22).

## Building and checking

`tools/dev/check`, from the repo, builds Hearth in the box and runs every check: the tests, qmllint, qmlformat, clang-tidy, clang-format and gitleaks. It prints PASS or FAIL for each. `tools/dev/check --fix` also reformats the code.

`build/tests/gallery/hearth-gallery` (in the box) shows every control in a window; add `high-contrast` for that theme.

## Repo guards

Set up by Claude on 2026-11-22, at the lead's request to make the repo safe for code. The lead reviews them.

### Git hooks (`.githooks/`, for everyone who commits)

Turn them on once per clone: `git config core.hooksPath .githooks` (done in the lead's clone).

| Hook | Refuses |
|---|---|
| `pre-commit` | Spike code (`spikes/`) on any branch but `spikes`; anything shaped like a secret (private keys, GitHub, AWS, Slack, Google, Anthropic and OpenAI-style tokens); your git email address or home-folder path in added lines. It also runs gitleaks, found on the path or inside `hearth-f45` |
| `pre-push` | Pushing the `spikes` branch anywhere; deleting `main` on GitHub; rewriting `main`'s history (a force push) |

Tested 2026-11-22 in a throwaway clone: each refusal fired, normal commits and pushes went through.

### Claude Code limits (`.claude/settings.json`, for AI sessions)

- A guard script ([.claude/hooks/git-guard.sh](../.claude/hooks/git-guard.sh)) runs before every shell command Claude makes:
  - Blocks force pushes, deleting branches on GitHub, pushing every branch at once, pushing `spikes`, skipping hooks (`--no-verify`), and pointing git's hooks anywhere but `.githooks`.
  - Asks the lead before any other push, and before GitHub commands that publish or change something: pull requests, releases, repo settings, secrets, and API calls that write.
- Blocked outright: the GitHub connector's tools that push several files, delete files or merge straight on GitHub, which would skip the git hooks entirely. Writing a single file, or creating a branch or pull request through it, asks first; a file written that way skips the git hooks' secret and personal-info checks.
- Asks first: any edit to `.githooks/` or `.claude/`, so the guards can't be loosened without the lead.
- Blocked: reading `.env`, `.pem` and `.key` files in the repo.

### Not covered yet

- Branch protection on GitHub: refused for a private repo on a free account; decision deferred (lead's log).
- Checks on GitHub's side (Actions running the same scans): wait for code.

## Spike code

Spikes are throwaway tests that answer one question. Their code lives on the local `spikes` branch, which is never pushed (agreed 2026-11-22). Results and every other documentation change are committed on `main` (the lead, 2026-11-22), then `main` is merged into `spikes` so the spike branch stays current. Until that day, documentation had been committed on `spikes` by mistake; fourteen commits were copied to `main`.

```sh
git switch spikes          # work on spike code
git switch -               # back to the branch you came from
```

`spikes` starts from `main` as it was on 2026-11-22. It has no copy anywhere else, so a lost disk loses it; that's the accepted cost.
