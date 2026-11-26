# Hearth UI guide (draft)

Started 2026-11-26. The principles come from the lead's calls in UI review rounds 1 and 2; the wording is Claude's, reviewed by the lead on 2026-11-26. It grows with each review round. Sizes, timings and decisions live in the [design doc](design.md); this page is the short version to design against.

## Rules every screen and theme follows

### Layout
1. **The home screen is a launcher, not a shop window.** No big featured banner, no category tabs, no side rail. Content lives in the apps.
2. **A calm home screen.** One row of apps by default, with a sliver of the next row showing. The background does most of the work.
3. **Status in one row at the top right,** clock and date at the far right. The profile shows as its picture, never its name.
4. **Panels dock to a screen edge.** Menus, keyboards and the volume bar attach to an edge; they never float. Short yes/no questions are the exception: they sit in the centre.
5. **Never add columns.** Going deeper adds rows where you are, or replaces the page.

### Look
6. **Every tile has colour by default:** frosted glass tinted with the app's own colour.
7. **Blur and see-through panels are part of the look,** with a "Reduce transparency" switch.
8. **Focus is unmistakable:** a strong fill or ring and a slight grow. Single keys get a round highlight.
9. **Variants match in size.** Alphabet and QWERTY keys are identical. Text is never smaller than 24 px at 1080p. Exceptions are kept to a minimum.

### Behaviour
10. **Show unavailable options, don't hide them:** greyed out, with one line saying why.
11. **Risky changes undo themselves.** A new resolution asks to be kept and goes back by itself if nobody answers.
12. **Errors speak plainly:** what happened (only what Hearth knows), what still works, what to do. The code is small, for support.
13. **Private by default.** A guest's logins are cleared unless the guest says keep; nobody answering counts as clear.
14. **The admin controls shared things:** turning the guest on, the guest PIN, possibly importing profile pictures.
15. **The keyboard keeps the text with it:** what's being typed and the word suggestions sit in the keyboard panel.

### Already agreed elsewhere
- Remote first: everything works with arrows, OK and Back.
- Designed at 1920×1080 and scaled for 4K (×2).
- TV-safe margins: 54–60 px top and bottom, 80–96 px at the sides.
- Motion about 200 ms; reduced motion swaps slides and zooms for fades.
- Focus never follows a mouse pointer.

## Colours
No colour is fixed yet. Every colour in the mockups is a development placeholder. The one rule so far: unavailable options look greyed out.

## Choices a theme can make
Claude's proposal, from the lead's note (2026-11-26) that several alternate layouts suit themes:
- Tile style: tinted frosted (default), colourless frosted, a solid colour picked from the icon, or wide banner art mixed in.
- Which edge the menu docks to (also a user setting).
- The profile picker's background, separate from the home background.
- Art for app tiles (the lead, 2026-11-25).

Theme packs are data only and carry no fonts ([Theming](decisions/0016-theming-own-tokens-no-kirigami.md)).

## Mockups
The round 1 and 2 mockups are on a private claude.ai design canvas: home screen, profile picker, Settings with Display sub-pages, the menu over the home screen and over an app, three keyboards, the profile picture picker, the guest leave question, a failure screen and a volume stand-in. The lead's reactions: [visual references](research/V1-visual-references.md).

## Still to design
- Volume overlay (round 3).
- Notifications, and the menu in more detail.
- The gamepad-wheel keyboard.
- Profile picture crop controls.
