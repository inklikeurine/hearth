# B2 – Focus and remote navigation in four TV design guides
 
Read 2026-11-20, makers' own pages only. NOT FOUND = the page doesn't say.
 
| Source | Rule | Numbers given | Link |
|---|---|---|---|
| Google | Focused item can change scale, border/outline, glow, colour | Scale 1.025, 1.05 or 1.1x (not mapped to element types); glow 2–32 dp; outline/border width NOT FOUND | https://developer.android.com/design/ui/tv/guides/styles/focus-system |
| Google | D-pad moves focus to the nearest object in the pressed direction; override with `nextFocus*` only if the default fails | none | https://developer.android.com/training/tv/start/navigation.html |
| Google | Back goes to the previous destination; no confirmation screens, no loops. In a top menu: scroll to top, focus menu. In a left menu: focus the active item | none | same |
| Google | Wrap-around: no rule for lists. Only: for custom ordering, make last control loop to first | none | same |
| Google | Initial focus: always have an actionable item focused at start and when idle; which item NOT FOUND | none | same |
| Apple | Focused item rises, lights up, animates; system effects preferred, custom "only if absolutely necessary" | Five states (unfocused, focused, highlighted, selected, unavailable); scale/shadow values NOT FOUND | https://developer.apple.com/design/human-interface-guidelines/focus-and-selection |
| Apple | Swipe moves focus the same direction; every element must be focusable; no free pointer | none | https://developer.apple.com/design/human-interface-guidelines/remotes |
| Apple | Back opens the parent screen (top level: Home); hold Back for Home; games pause instead | none | same |
| Apple | Wrap-around: HIG silent. Older programming guide: if nothing lies in that direction, focus stays | none | https://developer.apple.com/library/archive/documentation/General/Conceptual/AppleTV_PG/WorkingwiththeAppleTVRemote.html |
| Apple | Initial focus: by default the focusable view closest to top-left; app can name a preferred view (archived page) | none | same |
| Samsung | Focus "clear and highly visible", same pattern across the app; focused must differ from selected | Pixel sizes, colours NOT FOUND | https://developer.samsung.com/smarttv/design/design-principles.html |
| Samsung | One focused object at a time; items on a grid, not diagonal; predictable movement; scroll indicator when list exceeds a page | none | https://developer.samsung.com/smarttv/design/ux-checklist.html |
| Samsung | Back/Return: required, must be clear; behaviour NOT FOUND | none | design-principles page above |
| Samsung | Wrap-around: NOT FOUND. Initial focus: NOT FOUND | none | both pages above |
| LG | Every object needs a selection effect (animation, highlight, colour or size change), "no exception" | none | https://webostv.developer.lge.com/distribute/app-self-checklist |
| LG | Apps must support 4-way keys + OK + Back and the pointer; arrow key switches pointer to 5-way, shaking switches back; holding an arrow speeds list scrolling | none | https://webostv.developer.lge.com/develop/guides/magic-remote |
| LG | Back: previous page; at entry page, exit popup (webOS 6.0+) or Home launcher (5.0−); long press exits to Home | key code 461 | https://webostv.developer.lge.com/develop/guides/back-button |
| LG | Wrap-around: NOT FOUND. Initial focus: NOT FOUND | none | pages above |
 
## Notes
- LG's pages disagree: the checklist says Back on the entry page shows Home (webOS 23–25 only); the Back Button page says exit popup on 6.0+.
- Apple's "stays" rule and initial-focus rule come from an archived page; the current HIG doesn't repeat them.
- Google, Samsung and LG pages came through a summarizing fetch tool; spot-check quoted wording. Apple's pages were read as Markdown.
 
