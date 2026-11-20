# Hearth: hardware video decode on old PCs (final plan review)

> Read with the later decisions. The hardware floor has since moved to Intel 7th gen / AMD Raven Ridge, where VP9 decode starts, so 6th-gen Intel (Skylake) is now legacy tier, not "inside the floor". Firefox ships in the OS image, not as a Flatpak (design doc, section 2): the Flatpak findings below apply to Flatpak apps the user installs, and the image's own Firefox gets its codecs through the image's codec plan (design doc, section 5.3).

Written 2026-11-19. Every table cell points to a primary source (vendor docs, driver source code, upstream trackers). Anything I could not confirm from a primary source is marked UNCONFIRMED. Nothing here comes from memory.

## Summary

- Every Intel (4th gen+), AMD (UVD/VCN) and NVIDIA (Maxwell+) chip in range decodes H.264 in hardware.
- Intel 6th gen (Skylake) is the weak spot inside Hearth's floor: HEVC 8-bit only, no VP9, no HEVC 10-bit. On Skylake, YouTube must be forced to H.264.
- VP9 and HEVC 10-bit arrive with Intel 7th gen (Kaby Lake), AMD Raven Ridge (2018), NVIDIA Pascal. AV1 arrives with Intel 11th gen, and only in the non-free build of the Intel driver.
- Firefox Flatpak: the old advice (install `ffmpeg-full`) is outdated. Since Freedesktop 25.08 the codec extension is `codecs-extra` and installs automatically. Whether it gives *hardware* H.264 in Firefox is still UNCONFIRMED; test it on real hardware (Q3).

---

## 1. Decode table (VA-API unless noted)

"Yes/No" means hardware decode. Bit depth is only split where the source splits it.

### Intel

Sources: Intel oneVPL decode tables [I1], Intel 7th-gen+ article [I2], iHD per-platform flags in source [M8] [M9] [M11] [M12], iHD platform list [MR], i965 per-generation flags [P].

| Generation (codename) | H.264 | HEVC 8-bit | HEVC 10-bit | VP9 8-bit | VP9 10-bit | AV1 | VA-API driver |
|---|---|---|---|---|---|---|---|
| 4th (Haswell) | Yes [P] | No [P] | No [P] | No [P] | No [P] | No [P] | `libva-intel-driver` (i965) only; iHD starts at Broadwell [MR]. No Intel doc page read for Haswell, source code only. |
| 5th (Broadwell) | Yes [M8] | No [M8] | No [M8] | No [M8] | No [M8] | No [M8] | iHD [MR] or i965 [P] |
| 6th (Skylake) | Yes [M9] | Yes [M9] | No [M9] | No [M9] | No [M9] | No [M9] | iHD [MR] or i965 [P]. i965 has an optional "hybrid" VP9 build flag [P-NEWS]; not a normal hardware path. |
| 7th (Kaby Lake) | Yes [M9][I2] | Yes [M9][I2] | Yes [M9][I2] | Yes [M9][I2] | Yes [M9][I2] | No [I2] | iHD or i965 |
| 8th/9th (Coffee Lake, Whiskey Lake) | same as Kaby Lake: iHD reuses the Kaby Lake setup [M9] | | | | | | iHD or i965 |
| 10th (Comet Lake) | same as Kaby Lake; listed under the same platform group [MR] | | | | | | iHD or i965 |
| 10th (Ice Lake, mobile) | Yes [M11] | Yes [M11] | Yes [M11] | Yes [M11] | Yes [M11] | No [M11] | iHD only [MR] |
| 11th (Tiger Lake, Rocket Lake) | Yes [M12] | Yes [M12] | Yes [M12] | Yes [M12] | Yes [M12] | Yes, non-free build only [M12][MR]. Early Tiger Lake-LP steppings (`devRev == 0`) have AV1 turned off [M12]. AV1 bit depth: UNCONFIRMED. | iHD only |
| 12th (Alder Lake) | same silicon setup as Tiger Lake [M12] | | | | | | iHD only |

Cannon Lake and Gemini Lake exist in the sources but are outside your list.

What "non-free build" means: iHD compiles AV1 (and VC-1, HEVC encode) out when built with `ENABLE_NONFREE_KERNELS=OFF` [M-macro][MR]. Debian/Ubuntu ship both (`intel-media-va-driver` free, `intel-media-va-driver-non-free` full). Which one Fedora and the Flatpak ship: UNCONFIRMED.

### AMD APUs (driver: Mesa `radeonsi`)

Source for every row: X.Org RadeonFeature [X]. That page's UVD table has no VP9 or AV1 column, and it does not split 8-bit from 10-bit HEVC; it only adds a "10-bit noted" label on some rows.

| APU (year) | Decode engine [X] | H.264 | HEVC | VP9 | AV1 | Notes |
|---|---|---|---|---|---|---|
| Kaveri (2014), Godavari (2015) | UVD 4.2 | Yes | No | UNCONFIRMED (not on page) | No (not listed) | Godavari is not named on the page; I assume Kaveri silicon, UNCONFIRMED. |
| Carrizo (2015) | UVD 6.0 | Yes | Yes; 10-bit not noted for 6.0 | UNCONFIRMED | No (not listed) | I read "no 10-bit" from the missing note. That is an inference. |
| Bristol Ridge (2016) | not on page | UNCONFIRMED | UNCONFIRMED | UNCONFIRMED | UNCONFIRMED | No primary source found. |
| Stoney Ridge (2016) | UVD 6.2 | Yes | Yes, 10-bit noted | UNCONFIRMED | No (not listed) | |
| Raven Ridge (2018) | VCN 1.0 ("RAVEN") | Yes | Yes, 10-bit noted | Yes (bit depth not split) | No | Page does not say "RAVEN" means Raven Ridge, but it is the VCN 1.0 APU. |
| Picasso (2019) | not on page | UNCONFIRMED | UNCONFIRMED | UNCONFIRMED | UNCONFIRMED | |
| Renoir (2020) | VCN 2.0 | Yes | Yes, 10-bit noted | Yes | No | |

Gap in my sourcing: I could not read Mesa's source (GitLab blocked from my workspace), so "Mesa `radeonsi` serves these chips" and Mesa's per-chip codec gating are UNCONFIRMED from primary. Mesa's docs site was also unreachable.

Distro catch (found earlier in this review): Fedora and openSUSE ship Mesa with H.264/HEVC/VC-1 VA-API stripped. RPM Fusion's `mesa-va-drivers-freeworld` puts it back. This matters for Hearth if the image is Fedora-based, but not for Flatpak apps that use the Freedesktop runtime's own Mesa (whether that Mesa has the codecs: UNCONFIRMED).

### NVIDIA (added on request; discrete GPUs, no APUs)

Source for decode capability: NVIDIA NVDEC Application Note, SDK 13.0 [N1]; per-architecture table in the SDK 12.2 programming guide [N2]. Driver: `nvidia-vaapi-driver`, which sits on top of NVDEC [NV].

| Generation | H.264 | HEVC 8-bit | HEVC 10-bit | VP9 8-bit | VP9 10-bit | AV1 |
|---|---|---|---|---|---|---|
| 1st-gen Maxwell | Yes | No | No | No | UNCONFIRMED | No |
| 2nd-gen Maxwell | Yes | "Select GPUs" (not named by NVIDIA) | "Select GPUs" (not named) | "Select GPUs" (not named) | UNCONFIRMED | No |
| Pascal | Yes | Yes | Yes | Yes | UNCONFIRMED (not in the capability table) | No |
| Volta / Turing | Yes | Yes | Yes | Yes | UNCONFIRMED | No |
| Ampere and newer | Yes | Yes | Yes | Yes | UNCONFIRMED | Yes |

Limits that apply to NVIDIA on Hearth, from the driver's README [NV] and Firefox source [FX]:
- Firefox's graphics blocklist blocks NVIDIA VA-API by default, so `media.hardware-video-decoding.force-enabled` is required [FX].
- Needs NVIDIA driver 470 or 500+ and `nvidia-drm.modeset=1`; `NVD_BACKEND=direct` on 525+ [NV].
- Linux driver branch 590 dropped Maxwell/Pascal/Volta (580.105.08 was the last to support them). I did not re-check this source in this session.
- Firefox Flatpak + NVIDIA: a user report says hardware decode stayed off even with the same settings [FED]. No upstream fix confirmed. UNCONFIRMED.

---

## 2. What YouTube, Twitch and Dropout send to Firefox on Linux

I found no primary source that lists what each site sends to Firefox on Linux. Sites ask the browser what it can play and pick from the answer. So what you get depends on what Firefox claims. Test with YouTube's "Stats for nerds" and Firefox's `about:support`.

| Site | What primary sources say | Gap |
|---|---|---|
| YouTube | VP9 is used for 4K [YT]; AV1 is also in use. | Whether H.264 is offered at every resolution when VP9/AV1 are refused: UNCONFIRMED from a primary source. h264ify's whole design assumes it is [H264IFY]. |
| Twitch | H.264, plus HEVC for 1440p/2160p per Mozilla's tracker [B1962929] and Twitch's blog [TW]. | What Linux Firefox viewers actually receive: UNCONFIRMED. |
| Dropout | Hosted on Vimeo Streaming; Vimeo transcodes to H.264 [VIM]. | Actual codecs on the stream: UNCONFIRMED. |

### How an extension forces H.264

The browser answers three questions about codecs. h264ify-style extensions lie to the first two [H264IFY]:

1. `MediaSource.isTypeSupported(type)`
2. `HTMLMediaElement.canPlayType(type)`
3. `navigator.mediaCapabilities.decodingInfo(config)` (h264ify does not wrap this one; sites may use it)

The patch has to run in the page's own JavaScript world. An extension's normal content script cannot change page APIs, so it needs `"world": "MAIN"` [MDN]. Minimum Firefox version for that: UNCONFIRMED.

Example (my own sketch based on h264ify's approach; not tested):

```json
// manifest.json
"content_scripts": [{
  "matches": ["*://*.youtube.com/*", "*://*.twitch.tv/*", "*://*.vimeo.com/*"],
  "js": ["no-vp9-av1.js"],
  "run_at": "document_start",
  "world": "MAIN"
}]
```

```js
// no-vp9-av1.js
const BLOCK = /vp8|vp9|vp09|av01|webm/i;

const isSup = MediaSource.isTypeSupported.bind(MediaSource);
MediaSource.isTypeSupported = t => BLOCK.test(t) ? false : isSup(t);

const canPlay = HTMLMediaElement.prototype.canPlayType;
HTMLMediaElement.prototype.canPlayType = function (t) {
  return BLOCK.test(t) ? '' : canPlay.call(this, t);
};

const decInfo = navigator.mediaCapabilities.decodingInfo.bind(navigator.mediaCapabilities);
navigator.mediaCapabilities.decodingInfo = cfg =>
  BLOCK.test(cfg?.video?.contentType || '')
    ? Promise.resolve({ supported: false, smooth: false, powerEfficient: false })
    : decInfo(cfg);
```

For Hearth: turn this on only when the boot-time `vainfo` check finds no VP9/AV1 decode. On a GPU that does decode VP9, forcing H.264 throws away 4K.

Without an extension: Firefox pref `media.av1.enabled=false` turns off AV1; the VP9 pref name I had (`media.mediasource.vp9.enabled`) is UNCONFIRMED (Searchfox blocked). Prefs can be shipped through `policies.json` [POL]; which prefs that file allows: UNCONFIRMED.

---

## 3. Does H.264 hardware decode work in the Firefox Flatpak on Intel?

Probably yes on the current build, but I could not confirm it from a primary source. Test before you rely on it.

What is confirmed:
- The Firefox Flatpak runs on Freedesktop runtime 25.08 and has GPU device access (`devices: all`) [FH]. It declares no codec or VA-API extension of its own [FH].
- Freedesktop SDK 25.08 replaced `ffmpeg-full` and `openh264` with a `codecs-extra` runtime extension that installs automatically with the runtime [FD-REL][FD-DISC][FD-BLOG]. It holds FFmpeg's patented decoders plus x264/x265 [FD-BLOG].
- 25.08 also removed `intel-vaapi-driver`; the replacement is `intel-media-driver` [FD-DISC]. So in the Flatpak, Intel VA-API is iHD only. That means Haswell and older get no Intel VA-API in a 25.08 Flatpak. Hearth's floor is 6th gen, so this does not hurt you.
- Before 25.08, Mozilla's tracker says Firefox fell back to software OpenH264 unless a matching-version `ffmpeg-full` was installed [B1807977]. Support-forum answers say the extension version must match the runtime [MOZSUP].

What I could not confirm:
- Whether Firefox (built by Mozilla, not by Flathub) loads the FFmpeg inside `codecs-extra`. The only forum guides I found still say `ffmpeg-full` 24.08 [FLATHUB-FORUM].
- Whether `codecs-extra` FFmpeg is built with VA-API. The maintainer's write-up lists only FFmpeg's internal decoders and x264 [FD-BLOG].
- Whether `org.freedesktop.Platform.VAAPI.Intel` is installed automatically and whether its iHD is the free or non-free build (this decides AV1 on 11th gen+). Flathub's metadata has no driver details [VAAPI-FH].
- Whether `media.ffmpeg.vaapi.enabled` is on by default in Firefox on Linux. Firefox's blocklist does not block Intel [FX], but the default value itself is UNCONFIRMED.
- The extension ID spelling: the Discourse post uses `org.freedesktop.Platform.codecs_extra.i386` (underscore) and the KDE issue writes `codecs-extra` (hyphen). Check with `flatpak search codecs` on the real image.

What Hearth's image should do: install the 25.08 `codecs-extra` and `VAAPI.Intel` extensions at build time (no network needed at first boot), set the VA-API pref, and run a first-boot test that opens `about:support` data or logs with `MOZ_LOG=PlatformDecoderModule:5` and fails the image build if H.264 is not hardware-decoded on the reference Intel box.

---

## 4. Where this contradicts common forum advice

1. "Install `org.freedesktop.Platform.ffmpeg-full`." Outdated for Firefox on 25.08. It was replaced by `codecs-extra`, which installs automatically [FD-REL]. Most guides still say 24.08 [FLATHUB-FORUM][KDE-NV].
2. "Old Intel needs libva-intel-driver (i965)." Not in a 25.08 Flatpak: it was removed [FD-DISC]. And i965 is discontinued upstream [P].
3. "Skylake does HEVC 10-bit / VP9 in hardware." iHD's own Skylake flags say no to both [M9]. The Intel 7th-gen article starts the VP9/HEVC-10 list at Kaby Lake [I2].
4. "AV1 works on Tiger Lake / Alder Lake with iHD." Only with the non-free build. Free builds compile AV1 out [M12][MR].
5. "AMD works out of the box with Mesa." Not on Fedora/openSUSE: patent codecs are stripped from Mesa's VA-API (found earlier in this review; RPM Fedora's freeworld package fixes it).
6. "For NVIDIA, just set `media.ffmpeg.vaapi.enabled`." Not enough. Firefox blocks NVIDIA VA-API [FX] and you need `nvidia-vaapi-driver` plus the settings in section 1 [NV].
7. "h264ify forces H.264." It wraps two of the three codec questions. It does not wrap `MediaCapabilities` [H264IFY], and in a modern extension it needs the MAIN world [MDN].
8. "Twitch is just H.264." Twitch also serves HEVC at 1440p/2160p [B1962929][TW].

---

## Sources

Intel:
[I1] Intel oneVPL, Media Capabilities Supported by Intel Hardware: https://www.intel.com/content/www/us/en/docs/onevpl/developer-reference-media-intel-hardware/1-1/overview.html
[I2] Intel, encode and decode capabilities for 7th generation Core and newer: https://www.intel.com/content/www/us/en/developer/articles/technical/encode-and-decode-capabilities-for-7th-generation-intel-core-processors-and-newer.html
[MR] intel/media-driver README (supported platforms, free vs full build): https://github.com/intel/media-driver/blob/3043e7c82a591f9c876b96b9f52dfdeb53818217/README.md
[M8] Broadwell flags: https://github.com/intel/media-driver/blob/3043e7c82a591f9c876b96b9f52dfdeb53818217/media_driver/linux/gen8/ddi/media_sku_wa_g8.cpp
[M9] Skylake/Kaby Lake/Apollo Lake flags: https://github.com/intel/media-driver/blob/3043e7c82a591f9c876b96b9f52dfdeb53818217/media_driver/linux/gen9/ddi/media_sku_wa_g9.cpp
[M11] Ice Lake flags: https://github.com/intel/media-driver/blob/3043e7c82a591f9c876b96b9f52dfdeb53818217/media_driver/linux/gen11/ddi/media_sku_wa_g11.cpp
[M12] Tiger Lake/Rocket Lake/Alder Lake flags: https://github.com/intel/media-driver/blob/3043e7c82a591f9c876b96b9f52dfdeb53818217/media_driver/linux/gen12/ddi/media_sku_wa_g12.cpp
[M-macro] Free-build macro: https://github.com/intel/media-driver/blob/3043e7c82a591f9c876b96b9f52dfdeb53818217/media_softlet/linux/common/os/linux_system_info.h
[P] i965 per-generation flags: https://github.com/intel/intel-vaapi-driver/blob/master/src/i965_device_info.c (repo README announces discontinuation)
[P-NEWS] i965 NEWS (hybrid VP9 flag): https://github.com/intel/intel-vaapi-driver/blob/master/NEWS

AMD: [X] https://www.x.org/wiki/RadeonFeature/

NVIDIA:
[N1] https://docs.nvidia.com/video-technologies/video-codec-sdk/13.0/nvdec-application-note/index.html
[N2] https://docs.nvidia.com/video-technologies/video-codec-sdk/12.2/nvdec-video-decoder-api-prog-guide
[NV] https://github.com/elFarto/nvidia-vaapi-driver
[FED] Fedora Discussion, NVIDIA + Flatpak Firefox: https://discussion.fedoraproject.org/t/can-i-make-hardware-decoding-work-in-flathubs-flatpak-firefox-on-an-nvidia-gpu/146027

Firefox / Mozilla:
[FX] Firefox graphics blocklist source (mozilla-central `widget/gtk/GfxInfo.cpp`): https://github.com/mozilla/gecko-dev/blob/master/widget/gtk/GfxInfo.cpp
[B1807977] https://bugzilla.mozilla.org/show_bug.cgi?id=1807977
[B1962929] https://bugzilla.mozilla.org/show_bug.cgi?id=1962929
[MOZSUP] https://support.mozilla.org/bs/questions/1436190
[POL] https://mozilla.github.io/policy-templates/
[MDN] https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/manifest.json/content_scripts

Sites:
[YT] https://support.google.com/youtube/answer/1722171
[TW] Twitch Enhanced Broadcasting announcement (Twitch blog; exact URL not kept, re-add)
[VIM] Vimeo Help/blog pages on transcoding (exact URLs not kept, re-add)
[H264IFY] h264ify `inject.js`: https://github.com/erkserkserks/h264ify/blob/master/src/inject/inject.js

Flatpak / Freedesktop:
[FH] Flathub summary, Firefox: https://flathub.org/api/v2/summary/org.mozilla.firefox
[VAAPI-FH] https://flathub.org/api/v2/summary/org.freedesktop.Platform.VAAPI.Intel
[FD-REL] https://gitlab.com/freedesktop-sdk/freedesktop-sdk/-/releases/freedesktop-sdk-25.08.0
[FD-DISC] https://discourse.flathub.org/t/freedesktop-sdk-25-08-0-released/10399
[FD-BLOG] https://bbhtt.in/posts/closing-the-chapter-on-openh264/
[FLATHUB-FORUM] https://discourse.flathub.org/t/how-to-enable-video-hardware-acceleration-on-flatpak-firefox/3125
[KDE-NV] https://community.kde.org/KDE_Linux/Enable_hardware_acceleration_for_Firefox_on_NVIDIA_GPUs

Not read (blocked or unreachable from my workspace): Mesa source and docs, Mozilla's Flatpak manifest, the Flathub Firefox repo, Searchfox.
