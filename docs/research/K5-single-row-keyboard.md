# Single-row keyboard: moving highlight or scrolling row (K5)

Researched 2026-11-26 in a separate chat, summary copied from the claude.ai project. Tags: [C] primary source, [S] secondary, [M] memory.

## The lead decided (2026-11-26)
- Keep the moving highlight on a fixed row.
- Wrap at the ends: yes.
- Held arrow: plain repeat, one key at a time.

## Claude's proposal (as researched)
- **Options:** A) fixed row, the highlight moves. B) highlight fixed in the centre, letters scroll past.
- **Pick: A**, with wrap at the ends. No maker documents B, it moves least, and reduced motion costs nothing. B's one real edge: bigger keys.
- **Hard to undo:** little. Both are small changes in one QML component; centring is `highlightRangeMode`, wrap is `keyNavigationWraps` [C: [Qt ListView](https://doc.qt.io/qt-6/qml-qtquick-listview.html)].

## Confirmed
- tvOS gives Siri Remote users a linear keyboard, other devices a grid [C: [HIG](https://developer.apple.com/design/human-interface-guidelines/virtual-keyboards)]. Apple doesn't say whether the highlight moves or the row scrolls, or whether it wraps [C: [Apple guide](https://developer.apple.com/library/archive/documentation/General/Conceptual/AppleTV_PG/CreatingaGreatTextInputExperience.html)].
- Google TV, Roku, PlayStation, Xbox: no maker page describes a single-row or scrolling keyboard ([TV keyboard layouts](K4-tv-keyboard-layouts.md)). Fire TV says only to keep typing minimal [C: [Amazon](https://developer.amazon.com/public/solutions/devices/fire-tv/docs/design-and-user-experience-guidelines)].
- No study compares the two. Nearest: 20 people, arrow pad plus on-screen keyboard, no errors reported [C: [Choi 2016](https://uxpajournal.org/wp-content/uploads/sites/7/pdf/JUS_Choi_May2016.pdf)]. 10 people, about 0.5 s per press, no WPM or errors [C: [Google 2010](https://research.google.com/pubs/archive/36239.pdf)]. Cursor-style TV typing is about 8 WPM [S: [arXiv 2025](https://arxiv.org/html/2503.02133v1)].
- Patents describe a moving highlight ([US 11,755,199](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/11755199)) and a looping carousel ([US 9,134,881](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/9134881)) [C]. Claims not checked; not legal advice.
- WCAG 2.3.3 (AAA): motion from interaction must be switchable off unless essential. The user's own scrolling is allowed; added decoration isn't [C: [W3C](https://www.w3.org/WAI/WCAG22/Understanding/animation-from-interactions.html)].
- tvOS focus stays put when nothing lies in that direction [C: [Apple](https://developer.apple.com/documentation/uikit/about-focus-interactions-for-apple-tv)]. Qt lists don't wrap unless told to [C].

## Unconfirmed
- Whether tvOS's row moves or scrolls [M: highlight moves].
- Speed and error differences: nothing measured.
- Screen reader: a fixed row gives each key a stable place; a scrolling row may recycle items (reasoning, untested with Orca).
- Held arrow: acceleration untested; NEC infrared long-press is unreliable (design doc).
- Whether 26 letters fit one row at 1080p inside the safe margins: the round 2 mockup fits them at 52 px keys.

## Conflicts
- **Reduced motion (1.0):** a scrolling row would need its own no-slide mode. A moving highlight doesn't.
- **Remote-first:** none. Test repeat on a real infrared remote.
