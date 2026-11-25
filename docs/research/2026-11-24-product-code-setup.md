# Product code setup: proposal and test

*Claude's proposal, 2026-11-24, for the milestone "Ready to code". The lead agreed to it the same day, packages included. Tested with a throwaway project outside the repo; no product code was added.*

*Tags: [T] = run in the dev box (`hearth-f45`) on 2026-11-24; [C] = read in a primary source; [M] = from memory.*

## The proposal

| Choice | Claude's pick | Other options | Hard to undo later? |
|---|---|---|---|
| Repo layout | One repo, one folder per part under `src/`, tests beside them in `tests/` | One repo per part: more upkeep for one developer | Somewhat: moving files later makes history harder to follow |
| Build system | CMake with Ninja | Meson: Qt's own QML module tools are CMake functions [M] | Yes |
| Language | QML for screens, C++20 for the rest | Rust or Python for services: a second toolchain to learn and check | Yes |
| Tests | Qt Test for C++, Qt Quick Test for QML, both run by `ctest` with no screen; the spike's labwc checks become a third, slower set | Catch2 or GoogleTest: another dependency, and neither tests QML | No |
| Checkers | qmllint, qmlformat, clang-tidy, clang-format, gitleaks | clazy (Qt-specific C++ checks, in Fedora 45): can be added later | No |
| Where the checks run | One script, `tools/dev/check`, run by hand now and by GitHub Actions once CI exists | Hosted reviewers: still parked | No |

Layout:

```
src/
  shell/        home screen, overlays, menu, key handling, notifications
  keyboard/     on-screen keyboard
  bridge/       the Firefox extension and its native-messaging host
  services/     profiles, updates, hardware check
  settings/     settings app and first-boot wizard
image/          mkosi configuration
tests/          cpp/, qml/, session/ (headless labwc)
tools/dev/      dev box, check script
docs/
```

Folders appear when their first file does. `image/`, `src/services/` and `src/settings/` wait for their milestones.

## What was tested

A QML module with one control, an executable that uses it, one C++ test and one QML test, with CMake 4.3.0 and Qt 6.11.2.

| Check | Good code | Code broken on purpose |
|---|---|---|
| Build | Passes [T] | Not tried |
| C++ test | Passes [T] | Fails, as it should [T] |
| QML test, no screen | Passes [T] | Fails, as it should [T] |
| qmllint | Clean [T] | Caught, but see below [T] |
| clang-tidy with warnings as errors | Clean [T] | Catches a null pointer [T] |
| clang-format | Flagged a brace/spacing style; the style has to be chosen and the code formatted with it [T] | — |
| qmlformat | Two of three files unchanged; it wants to reformat the hand-written test [T] | — |
| gitleaks | Clean [T] | Not tried |

Found:
- Qt's built-in lint target (`all_qmllint`) exits 0 when it finds warnings, so it can't stop bad code. `qmllint -W 0` exits with an error [T]. The check script has to use that.
- Everything needed for this is already in the dev box except KI18n (`kf6-ki18n-devel`, for Hearth's translated text), which Fedora 45 has at 6.32.0 [T].

## Packages (approved)

| Package | For |
|---|---|
| `kf6-ki18n-devel` | Translations (already agreed as KI18n) |
| `libxkbcommon-devel` | Key names in the on-screen keyboard. Already in the dev box because Qt pulls it in [T]; the spike was told not to link it without approval |
| `qrencode-devel` | QR codes on failure screens |
| `hunspell-devel`, `hunspell-en-US` | Word suggestions |

## Not confirmed

- How these checks behave on GitHub Actions: no CI exists yet.
- Whether the spike's headless labwc checks run inside `ctest` without changes.
