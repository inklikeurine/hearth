# A7 – UI white on an HDR screen
 
**Answer:** BT.2408 puts graphics white at 203 nits. GNOME's default is 203. KDE's code default is 200 (a KWin developer says new configs start at 500).
 
Tags: [C] primary read, [S] secondary.
 
| Source | What it says | Number (nits) | Link |
|---|---|---|---|
| BT.2408-9 §2.1, Table 1 [C] | HDR Reference White is a nominal level on PQ, or on HLG with a 1,000-nit peak. Graphics White has the same signal level (58% PQ, 75% HLG). | 203 | [ITU PDF](https://www.itu.int/dms_pub/itu-r/opb/rep/R-REP-BT.2408-9-2026-PDF-E.pdf) |
| BT.2408-9 §9, Annex 8, 9.1 [C] | §9 maps SDR graphics to that level but gives no nits. Annex 8 shows 203 on a 1,000-nit HLG monitor; on a 600-nit HLG monitor reference white shows 138. | 203 (138) | same |
| KWin source [C] | Output state starts with `referenceLuminance = 200`. | 200 | [backendoutput.h](https://github.com/KDE/kwin/blob/master/src/core/backendoutput.h) |
| KWin setting [S] | Display settings, Plasma 6.3+: "Maximum SDR Brightness" (brightness slider at 100%). | not stated | [KWin dev blog](https://planet.kde.org/xavers-blog-2025-03-31-on-brightness-and-calibrating-your-displays) |
| KWin default for new configs [S] | KWin developer: new outputs default to 500, or the display's maximum if lower. | 500 | [bug 494134](https://bugs.kde.org/show_bug.cgi?id=494134) |
| Mutter [C] | Default sRGB white is 203 (property `srgb-white-luminance`, previously hard-coded). Changeable via Looking Glass (GNOME's debug tool). | 203 | [MR 3886](https://gitlab.gnome.org/GNOME/mutter/-/merge_requests/3886) |
| GNOME user setting [S] | A forum poster says there is no HDR brightness slider. | NOT FOUND | [forum](https://discourse.gnome.org/t/configuring-hdr-luminance/32936) |
 
**Recommendation (not a decision):** default Hearth's UI white to 203 nits in HDR mode, adjustable.
 
## Not confirmed
- KWin's 200 vs 500 conflict: not resolved in source.
- Mutter's value is from a merge request; current main not checked (fetch failed).
- A TV's real output at 203 needs testing.
 
