# Decision 0019: Heavy accessibility features for v2.0.0; zoom and the simple layout dropped

Replaces decision 0013. Related: [labwc for 1.0.0](0015-labwc-for-1-0.md), [Theming](0016-theming-own-tokens-no-kirigami.md).

## Why it changed

Decision 0013 listed screen zoom and a separate simple layout under 2.0.0 while calling them "unwanted" and "unnecessary". The lead, 2026-11-24: "Zoom and the simple layout: drop both." The rest is carried over.

## Decision

| Feature | Release | Why |
|---|---|---|
| Screen reader (Orca), with the "hold Back for 5 seconds" shortcut at first boot | 2.0.0 | Needs every screen labelled and tested, plus compositor support |
| Color filters for color blindness | 2.0.0 | A compositor feature |
| Dialogue boost | 2.0.0 | Needs a tuned audio filter |
| Text size, high-contrast themes, reduced motion | 1.0.0 | Theme tokens ([Theming](0016-theming-own-tokens-no-kirigami.md)) |
| Mono audio, balance | 1.0.0 | Small sound settings |
| Slower key repeat, ignore accidental double presses, longer hold times | 1.0.0 | Key-handling settings |
| Never use color as the only signal; no more than three flashes a second ([WCAG 2.3.1](../research/A3-wcag22-numbers.md)) | 1.0.0 | Design rules |

- Captions are left to each app, not the OS.
- Every control gets an accessible name and role (Qt's `Accessible.name` and `Accessible.role`) from 1.0.0, so the 2.0.0 screen reader doesn't need every screen reopened.

## Consequences

- The compositor used in 1.0.0 must gain screen-reader support by 2.0.0, or Hearth changes compositor then.

## Turned down

- A screen reader in 1.0.0: too big to get right alongside everything else.
- Dropping all the heavy features: only zoom and the simple layout go.
- Screen zoom: unwanted.
- A separate simple layout: unnecessary, Hearth's layout is simple by design.
