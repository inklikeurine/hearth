# A3: WCAG 2.2 numbers
 
Read 2026-11-20, w3.org only: the [WCAG 2.2 standard](https://www.w3.org/TR/WCAG22/) for wording and definitions, Understanding pages for numbers. All confirmed. Ratios are not rounded: 4.499:1 fails 4.5:1.
 
| Criterion | Level | Rule in one sentence | Exact numbers | Link |
|---|---|---|---|---|
| 1.4.3 Contrast (Minimum) | AA | Text must contrast enough with its background. | Normal text 4.5:1. Large text 3:1 (at least 18 pt, or 14 pt bold; about 24 px and 18.5 px). Exempt: inactive controls, decoration, logos. | [page](https://www.w3.org/WAI/WCAG22/Understanding/contrast-minimum.html) |
| 1.4.6 Contrast (Enhanced) | AAA | Same, stricter. | Normal text 7:1. Large text 4.5:1. Same size definition and exemptions. | [page](https://www.w3.org/WAI/WCAG22/Understanding/contrast-enhanced.html) |
| 1.4.11 Non-text Contrast | AA | Controls, their states (including focus) and essential graphics must stand out from neighbouring colours. | 3:1. Exempt: disabled controls, browser-default looks, logos. | [page](https://www.w3.org/WAI/WCAG22/Understanding/non-text-contrast.html) |
| 2.4.7 Focus Visible | AA | Keyboard users must be able to see which item has focus. | None. | [page](https://www.w3.org/WAI/WCAG22/Understanding/focus-visible.html) |
| 2.4.11 Focus Not Obscured (Minimum) | AA | A focused item may not be entirely hidden by author-made content, such as a sticky bar. | None; partly covered is allowed. | [page](https://www.w3.org/WAI/WCAG22/Understanding/focus-not-obscured-minimum.html) |
| 2.4.13 Focus Appearance | AAA | The focus indicator must be thick enough and clearly different from the unfocused look. | Area at least a 2 CSS px thick perimeter of the unfocused item; 3:1 change between focused and unfocused pixels. Example: a 90×30 px button needs 480 px². Exempt: indicators the author doesn't control or change. | [page](https://www.w3.org/WAI/WCAG22/Understanding/focus-appearance.html) |
| 2.3.1 Three Flashes or Below Threshold | A | Nothing flashes more than three times in any one second, unless it stays under the flash thresholds. | General flash: luminance change of 10% or more of the maximum (1.0), darker state below 0.80. Red flash: a pair of changes involving saturated red. Safe area: flashes total at most 0.006 steradians in any 10° field (about 25% of it). | [page](https://www.w3.org/WAI/WCAG22/Understanding/three-flashes-or-below-threshold.html) |
| 2.3.3 Animation from Interactions | AAA | Motion animation triggered by interaction can be turned off, unless essential. | None. | [page](https://www.w3.org/WAI/WCAG22/Understanding/animation-from-interactions.html) |
 
## How the contrast ratio is calculated
The ratio is (L1 + 0.05) / (L2 + 0.05), where L1 is the relative luminance (perceived brightness, 0 to 1) of the lighter colour and L2 of the darker one. Range: 1:1 to 21:1. Luminance is L = 0.2126 R + 0.7152 G + 0.0722 B. Each channel is first divided by 255 to give s; then R = s / 12.92 if s is 0.04045 or less, otherwise R = ((s + 0.055) / 1.055)^2.4. Example, white text on #000000: white has all channels at s = 1, which stays 1, so L = 0.2126 + 0.7152 + 0.0722 = 1. Black has all channels at 0, so L = 0. Ratio = (1 + 0.05) / (0 + 0.05) = 21, the maximum, passing every threshold above.
 
