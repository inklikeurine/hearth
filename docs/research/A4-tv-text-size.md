# A4 – Minimum text size on TV design guides
 
**Answer:** Google: 12sp = 24 px. BBC: 24 px, screen size unstated. Apple: 23 pt = 23 px if 1 pt = 1 px (unconfirmed).
 
Tags: [C] primary source read, [M] memory.
 
| Source | Smallest body text | Unit | Resolution assumed | Viewing distance assumed | Link |
|---|---|---|---|---|---|
| Google, legacy "Style for TV" page | 12sp (default 18sp) [C] | sp | 1920x1080 [C] | about 10 ft [C] | [[C]](https://developer.android.com/design/tv/style.html) |
| Google, current TV typography page | NOT FOUND | NOT FOUND | NOT FOUND | NOT FOUND | [[C]](https://developer.android.com/design/ui/tv/guides/styles/typography) |
| Google, Design for TV page | no text size | n/a | n/a | 3 m (10 ft) average [C] | [[C]](https://developer.android.com/design/ui/tv/guides/foundations/design-for-tv) |
| Apple tvOS HIG, typography | 23 pt (default 29 pt) [C] | pt | NOT FOUND | "often 8 feet or more" [C] (Designing for tvOS page) | [[C]](https://developer.apple.com/design/human-interface-guidelines/typography) |
| BBC GEL Connected TV typography (2012 blog image) | 24 px body copy, bold [C]; 21 px only for uppercase [C] | px | NOT FOUND | NOT FOUND | [[C]](https://www.bbc.co.uk/blogs/internet/entries/69be18b5-a5d4-3996-9d1f-f45690533b37) |
 
## Conversion to pixels on 1920x1080
 
**Google.** On Android TV, a 1080p screen is the xhdpi bucket [C]([layouts](https://developer.android.com/training/tv/playback/compose/layouts)). xhdpi is 2.0x, and px = dp x (dpi / 160) [C]([densities](https://developer.android.com/training/multiscreen/screendensities)).
- 12sp x (320 / 160) = 12 x 2 = **24 px**
- Default: 18sp x 2 = 36 px
- Assumes the user's system font scale is 1.0.
**Apple.** If 1 pt = 1 px on a 1080p tvOS screen:
- 23 pt x 1 = **23 px**
- Default: 29 pt = 29 px
- The 1 pt = 1 px step is [M]. Layout-page safe-area margins (60 pt top/bottom, 80 pt sides) [C]([layout](https://developer.apple.com/design/human-interface-guidelines/layout)) fit a 1920x1080 canvas, but no page states it.
**BBC.** The figure is already in px: **24 px**. The source doesn't say what screen size the px are measured on. If they are 1920x1080 px, the answer is 24 px. If BBC drew them on a 1280x720 canvas, 24 x (1920 / 1280) = 36 px at 1080p.
 
## Not confirmed
 
- Apple's screen resolution.
- BBC resolution and distance. The 24 px comes from an image in a 2012 BBC blog post (your saved copy). The GEL TV typography page it links to could not be opened (bbc.co.uk blocked).
- The Appt Accessibility Handbook has no TV text size.
- Google's 12sp is from the older page; current pages give no size.
- Test on a real 1080p TV from a sofa.
 
