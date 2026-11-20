# B3 – Screen layout at 1920×1080 in four TV guides
 
Read 2026-11-20, makers' own pages only. [C] = page read; [calc] = my arithmetic. All results in pixels at 1920×1080.
 
| Source | Edge margin | Grid | Spacing unit | Unit used | Link |
|---|---|---|---|---|---|
| Google | Sides 48dp × 2 = 96 px; top/bottom 27dp × 2 = 54 px (5% of 960×540) [C][calc]. Same page also gives 58dp = 116 px sides, 28dp = 56 px and 24dp = 48 px | 12 columns × 52dp = 104 px; gutter 20dp = 40 px; side margin 58dp = 116 px [C]. Check: 12×104 + 11×40 + 2×116 = 1920 [calc] | NOT FOUND (only 4dp line spacing = 8 px) | dp. Page: design at 960×540, 1px = 1dp, assets for 1080p, so ×2 [calc] | https://developer.android.com/design/ui/tv/guides/styles/layouts |
| Apple | HIG: top/bottom 60pt = 60 px; sides 80pt = 80 px [C]. WWDC19 talk says 90pt sides = 90 px | 2 to 9 columns; gap 40pt = 40 px; minimum row gap 100pt = 100 px [C]. Unfocused widths 2→9 columns: 860, 560, 410, 320, 260, 217, 184, 160pt. Check: 9×160 + 8×40 = 1760 = 1920 − 2×80 [calc] | NOT FOUND ("use consistent spacing"; nearest number is the 40pt gap) | pt. 1pt = 1 px at 1080p (WWDC16: design at 1920×1080, 1x assets; WWDC19: 1x for 1080p, 2x for 4K) | https://developer.apple.com/design/human-interface-guidelines/layout ; https://developer.apple.com/videos/play/wwdc2016/802/ ; https://developer.apple.com/videos/play/wwdc2019/211/ |
| Samsung | NOT FOUND | NOT FOUND (says use a grid, gives no numbers) | NOT FOUND | px. App resolution 1920×1080 px, so 1:1 [C] | https://developer.samsung.com/smarttv/design/apps-screen.html ; https://developer.samsung.com/smarttv/design/design-principles.html ; https://developer.samsung.com/smarttv/design/ux-checklist.html |
| LG | 20 px on all edges (webOS TV 6.0) [C]: 1.0% of width, 1.9% of height [calc]. Page names no resolution; 1920×1080 is the app default | NOT FOUND | NOT FOUND | px | https://webostv.developer.lge.com/develop/guides/overscan ; https://webostv.developer.lge.com/distribute/app-self-checklist |
 
## Notes
- Apple disagrees with itself: HIG says 80 on the sides; WWDC19 says 90; WWDC16 says "90 pixels from each side."
- Google's page gives 48 and 58dp for sides and 24, 27 and 28dp for top/bottom. It also says most modern TVs no longer have overscan issues.
- Google's ×2 comes from the page's own 960×540 canvas. It doesn't state the density of a real 1080p Android TV: test on hardware.
- Samsung and LG may give numbers only in images, which my fetch tool can't read. Google, Samsung and LG pages came through a summarizing fetch tool; WWDC pages were seen as search snippets only. Apple's HIG was read in full.
 
