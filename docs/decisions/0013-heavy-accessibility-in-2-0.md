# Decision 0013: Heavy accessibility features come in 2.0.0

Accepted 2026-11-22 by the lead. Related: design doc, sections 5.4, 9 and 14.

## Context

Section 14 of the design doc put a full accessibility set into 1.0, including a screen reader for every Hearth screen. The lead's reason for the change, in their words: the screen reader is "a huge undertaking ... requiring lots of research to get right", and it was "just an example" of the heavy features.

## Decision

The lead: "move all heavy lift accessibility options to 2.0.0, screen reader was just an example."

Which features count as heavy: Claude proposed the split below, and the lead agreed to it on 2026-11-22.

| Feature | Release | Why |
|---|---|---|
| Screen reader (Orca), with the "hold Back for 5 seconds" shortcut at first boot | 2.0.0 | Needs every screen labelled and tested, plus compositor support |
| Screen zoom | 2.0.0 | A compositor feature |
| Colour filters for colour blindness | 2.0.0 | A compositor feature |
| Dialogue boost | 2.0.0 | Needs a tuned audio filter |
| A separate simple layout | 2.0.0 | A second home-screen layout. Fewer rows and bigger tiles come in 1.0 through the rows-and-columns setting (section 18) |
| Text size, high-contrast themes, reduced motion | 1.0 | Theme tokens, already planned ([Theming](0009-theming-presets-and-packs.md)) |
| Captions on by default once chosen, caption size and colour | 1.0 | Settings in Hearth's own player; web apps use the sites' own captions |
| Mono audio, balance | 1.0 | Small sound settings |
| Slower key repeat, ignore accidental double presses, longer hold times | 1.0 | Settings in the input service Hearth writes anyway |
| Never use colour as the only signal; no more than three flashes a second ([WCAG 2.3.1](../research/A3-wcag22-numbers.md)) | 1.0 | Design rules |

Also **Claude's proposal**: every control gets an accessible name and role (Qt's `Accessible.name` and `Accessible.role`) from 1.0, so the 2.0 screen reader doesn't need every screen reopened.

## Consequences

- Dropped from 1.0: the rule "every new screen is checked with the screen reader". The high-contrast check stays.
- KWin's main advantage over labwc was screen-reader support ([compositor accessibility and HDR](../research/E3-compositor-a11y-hdr.md)). That now matters for 2.0, not 1.0.
- The compositor used in 1.0 must gain screen-reader support by 2.0, or Hearth changes compositor then.

## Turned down

- A screen reader in 1.0: too big to get right alongside everything else.
- Dropping the heavy features altogether.
