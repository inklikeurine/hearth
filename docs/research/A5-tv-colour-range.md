# A5 – TV colour range ("crushed blacks")
 
**Answer:** A range mismatch wrecks near-black greys. Full sent to a limited TV crushes them into black; limited sent to a full TV lifts black into grey. Intel and AMD share one kernel setting, "Broadcast RGB". NVIDIA has its own, X11-only.
 
Tags: [C] primary read, [S] secondary.
 
## 1. The two ranges
- **Full:** 0 = black, 255 = white. **Limited:** 16 = black, 235 = white; 0–15 and 236–255 are unused. [C] [kernel](https://github.com/torvalds/linux/blob/master/drivers/gpu/drm/drm_connector.c)
- TVs expect limited: HDMI says limited for all CEA video modes except 640x480. [S] [HDMI 1.4a §6.6](https://lists.freedesktop.org/archives/intel-gfx-bugs/2018-February/088420.html). CTA-861 text: NOT FOUND (paywalled).
Example grey: 10 (very dark).
 
| Mismatch | What happens | Look |
|---|---|---|
| PC full, TV expects limited | TV treats 16 as black; 10 is below that, clipped to 0 | Greys 0–16 all show as black; 236–255 all white |
| PC limited, TV expects full | PC sends 10 as 25; TV shows 25 as-is. Black goes out as 16 | Greys too bright; black is dark grey (~6%) |
 
Sony confirms these symptoms. [C] [Sony](https://www.sony.ca/en/electronics/support/articles/00013709)
 
## 2. Setting the range on Linux
 
| Driver | Setting name | Values | Default | Link |
|---|---|---|---|---|
| Intel (i915 and xe) | "Broadcast RGB" | Automatic, Full, Limited 16:235 | Automatic: limited on HDMI CEA modes, else full [C]. (0 = Automatic unconfirmed.) | [hdmi.c](https://github.com/torvalds/linux/blob/master/drivers/gpu/drm/i915/display/intel_hdmi.c), [connector.c](https://github.com/torvalds/linux/blob/master/drivers/gpu/drm/i915/display/intel_connector.c) |
| AMD (amdgpu) | "Broadcast RGB" | same | Automatic = full [C]. Patch to follow CTA default proposed, unmerged. | [patch](https://www.mail-archive.com/amd-gfx@lists.freedesktop.org/msg117199.html), [proposal](https://ratatoskr.run/lkml/2026/09/17548018/t) |
| NVIDIA (proprietary) | X option "ColorRange" (read back: CurrentColorRange) | Full, Limited | Full for RGB [C] | [README](https://download.nvidia.com/XFree86/Linux-x86_64/580.65.06/README/xconfigoptions.html) |
 
- xe builds the same display files, so same setting. [C] [Makefile](https://github.com/torvalds/linux/blob/master/drivers/gpu/drm/xe/Makefile). xe starts at Tiger Lake; Lunar Lake and Battlemage default to it. [S] [Phoronix](https://www.phoronix.com/news/Intel-Xe-More-Last-Minute-6.11). Older Intel: i915 only.
- YCbCr output is always limited; "Broadcast RGB" is ignored for it. [C] kernel link above
- X11 example: `xrandr --output HDMI-1 --set "Broadcast RGB" "Full"`. [S] [KDE bug 453594](https://bugs.kde.org/show_bug.cgi?id=453594)
## Not confirmed
- AMD kernel version that added the setting: NOT FOUND.
- NVIDIA under Wayland: NOT FOUND in docs; users report none. [S] [KDE forum](https://discuss.kde.org/t/use-hdmi-limited-colour-range-and-ycbcr444-on-wayland/41344)
- Which compositor sets the property for Hearth: not checked.
- All needs real-TV testing.
**Recommendation (not a decision):** offer a "TV colour range" setting (Auto/Full/Limited).
 
