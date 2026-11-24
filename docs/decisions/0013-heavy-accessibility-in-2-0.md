# Decision 0013: Heavy accessibility features for v2.0.0

Related: design doc, sections 5.4, 9 and 14.

## Context

Section 14 of the design doc put a full accessibility set into 1.0, including a screen reader for every Hearth screen.

## Decision

Move all "heavy lift" accessibility options to 2.0.0

### Which features count as heavy

| Feature | Release | Why |
|---|---|---|
| Screen reader (Orca), with the "hold Back for 5 seconds" shortcut at first boot | 2.0.0 | Needs every screen labelled and tested, plus compositor support |
| Screen zoom | 2.0.0 | unwanted |
| Color filters for color blindness | 2.0.0 | A compositor feature |
| Dialogue boost | 2.0.0 | Needs a tuned audio filter |
| A separate simple layout | 2.0.0 | unnecessary, hearth by design has a simple layout |
| Text size, high-contrast themes, reduced motion | 1.0.0 | Theme tokens, already planned ([Theming](0016-theming-own-tokens-no-kirigami.md)) |
| Captions on by default once chosen, caption size and colour | 1.0.0 | This is left to each app to handle, not the OS |
| Mono audio, balance | 1.0.0 | Small sound settings |
| Slower key repeat, ignore accidental double presses, longer hold times | 1.0.0 | Settings in the input service Hearth writes anyway |
| Never use colour as the only signal; no more than three flashes a second ([WCAG 2.3.1](../research/A3-wcag22-numbers.md)) | 1.0.0 | Design rules |

Also: every control gets an accessible name and role (Qt's `Accessible.name` and `Accessible.role`) from 1.0.0, so the 2.0.0 screen reader doesn't need every screen reopened.

## Consequences

- The compositor used in 1.0.0 must gain screen-reader support by 2.0, or Hearth changes compositor then.

## Turned down

- A screen reader in 1.0.0: too big to get right alongside everything else.
- Dropping the heavy features altogether.
