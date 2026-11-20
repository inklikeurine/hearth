# B4 – Animation timing, easing and reduced motion in four TV guides
 
Read 2026-11-20 (second pass), makers' own pages only. NOT FOUND = the page gives no number.
 
| Source | Animation type | Duration (ms) | Easing | Link |
|---|---|---|---|---|
| Google TV guide | All | NOT FOUND. Style section has Color, Typography, Focus system, Layouts; no motion page. TV app quality page has no motion criteria | NOT FOUND | https://developer.android.com/design/ui/tv/guides/styles/color-system ; https://developer.android.com/docs/quality-guidelines/tv-app-quality |
| Google, Material 3 (parent system, not TV-specific) | Selection controls | 200 (snippet only). Token scale 50 to 1000 | Standard: cubic-bezier(0.2, 0, 0, 1) | https://m3.material.io/styles/motion/easing-and-duration/tokens-specs ; https://raw.githubusercontent.com/material-components/material-components-android/master/docs/theming/Motion.md |
| Apple tvOS | Focus move | NOT FOUND. WWDC17: UIKit shortens duration when focus moves fast, lengthens it when slow (snippet) | NOT FOUND | https://developer.apple.com/videos/play/wwdc2017/224/ |
| Apple tvOS | Feedback animations | NOT FOUND. "Brief and precise"; let people cancel | NOT FOUND | https://developer.apple.com/design/human-interface-guidelines/motion |
| Samsung Smart TV | Loading animation | NOT FOUND for the animation. Progress indicator required for waits of 100 ms or longer; text message is an acceptable substitute if the animation slows the task | NOT FOUND | https://developer.samsung.com/smarttv/design/apps-screen.html ; https://developer.samsung.com/smarttv/design/design-principles.html |
| Samsung Smart TV | Control feedback | NOT FOUND. "Instant and distinct" | NOT FOUND | design-principles page above |
| LG webOS TV | Animation, transitions, focus | NOT FOUND. "Use animation and transitions"; selection effect required (animation, highlight, colour or size) | NOT FOUND | https://webostv.developer.lge.com/develop/guides/design-principles ; https://webostv.developer.lge.com/distribute/app-self-checklist |
| LG (2014 NetCast PDF) | Playback controls auto-hide | 5000 (timeout, not an animation) | NOT FOUND | https://webostv.developer.lge.com/assets/netcast/LG_Smart_TV_and_Media_Product_UI_Guideline_v4_18.pdf |
 
## Reduced motion
- **Google:** TV guide NOT FOUND. Material 3: with the setting on, use subtle fades, not intense sliding or scaling; drop parallax and shape morphing (snippet). Android's system animation-scale setting exists (Settings.Global.ANIMATOR_DURATION_SCALE) but isn't a TV rule.
- **Apple:** Accessibility page (applies to tvOS): when Reduce Motion is on, cut automatic and repeated animation, including zooming, scaling, peripheral motion. Also: tighter springs, no z-axis depth animation, fades instead of x/y/z transitions, no blur animation. Doesn't say if tvOS focus scaling counts. https://developer.apple.com/design/human-interface-guidelines/accessibility
- **Samsung:** No reduced-motion setting. Accessibility page asks only for a static option for moving text and minimal flicker. https://developer.samsung.com/smarttv/develop/guides/fundamentals/accessibility.html
- **LG:** NOT FOUND.
## Notes
- LG's menu lists "Style" and "System UI" pages with no URL; unread.
- Google/Samsung/LG pages came through a summarizing fetch tool; Material 3 and WWDC17 pages only as search snippets. Apple's HIG pages were read in full.
 
