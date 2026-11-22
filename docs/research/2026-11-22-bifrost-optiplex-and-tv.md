# What bifrost learned about the test boxes and the TVs

*Read by Claude on 2026-11-22, at the lead's request, from the lead's own bifrost repos (`bifrost-htpc` at `985e81b`, `bifrost-roku-bridge` at `39ea6fd`), their GitHub issues and pull requests. Bifrost is a source of ideas only; Hearth takes none of its setup or scripts.*

*Tags: **[T]** = tested on the lead's real hardware, as recorded in bifrost (where noted); **[B]** = written in bifrost's docs, with no test recorded; [C] = primary source; [S] = secondary source; [M] = Claude's memory.*

## The two boxes

| | OptiPlex HTPC | ThinkCentre HTPC |
|---|---|---|
| Model | Dell OptiPlex 3080 Micro (`D14U002`) [B] | Lenovo ThinkCentre M910q Tiny (`10MUS33K00`) [B] |
| CPU | Not recorded. The 3080 ships with anything from a Celeron to an i5 [B]; all 10th-gen Intel (Comet Lake) [M]. Check in the firmware setup (F2 → System Information) | Intel Core i5-7500T [B]: 7th gen (Kaby Lake), **the generation Hearth's floor names** |
| RAM | Not recorded | 8 GB (7.6 GiB usable) [B] |
| Graphics | Intel UHD 630 (i3/i5) or UHD 610 (Celeron/Pentium) [B]. Neither decodes AV1 in hardware [B] | Intel HD Graphics 630: hardware decode for H.264, HEVC 10-bit and VP9 10-bit; no AV1 [B] |
| Video out | Rear HDMI 1.4: 4K at up to 30 Hz [B, from Dell's spec sheet]. 4K at 60 Hz only over DisplayPort [S, Dell community]. HDMI 1.4 carries no HDR [M] | Not recorded |
| Network | M.2 2230 Wi-Fi slot, **no card fitted**; Intel AX200 or AX210 is the safe Linux pick; antenna cables may be missing (Dell's 3080/5080/7080 MFF antenna kit) [B, Dell service manual] | Wi-Fi only, no Ethernet connected [B] |
| Remote | Not recorded | WeChip W1 air mouse (USB dongle) [B] |
| TV | LG, on the LAN only [B] | Roku TV: TCL 49S405, Roku OS 16.0.4 [T, bifrost-roku-bridge testing, 2026-10-19] |

What this means for Hearth (Claude's reading):
- The OptiPlex with no Wi-Fi card is a ready test of "optional hardware never assumed". Bluetooth usually rides on the same M.2 card, so it likely has none either [M].
- The ThinkCentre's CPU and graphics are exactly the floor generation. Only its RAM (8 GB) is above the floor.

## Video decoding on Fedora 45

- Fedora's FFmpeg (`ffmpeg-free`) passes H.264 to OpenH264, and Fedora ships `noopenh264`, an empty stand-in. Fedora's Firefox (`firefox-160.0-1.fc45`, the same build as Hearth's dev box) therefore lists H.264 as supported, accepts the video, and fails when it starts playing ("This video can't play here."). [T, [issue #12](https://github.com/inklikeurine/bifrost-htpc/issues/12)]
- Fedora 45's own Intel video driver (`libva-intel-media-driver`) decodes VP9 and AV1 on the GPU, but not H.264 or HEVC. Checked with `vainfo` on Fedora 45. RPM Fusion's `intel-media-driver` adds H.264 and HEVC and installs alongside Fedora's, so nothing has to be removed. [T, [PR #14](https://github.com/inklikeurine/bifrost-htpc/pull/14)]
- AMD on Fedora 45: `mesa-va-drivers` no longer exists; its drivers moved into `mesa-dri-drivers`, and RPM Fusion's `mesa-va-drivers-freeworld` installs alongside it. [T, PR #14, checked in a Fedora 45 container]
- Replacing Fedora's FFmpeg with RPM Fusion's full one (`ffmpeg`) removes exactly eight `-free` packages, and nothing else depends on them. The list has to be read from the box: Kinoite 45 has no `libpostproc-free`. [T, PR #14]
- With RPM Fusion's FFmpeg and Intel driver, H.264 played and was hardware-decoded on the lead's box (v0.1.0, 2026-11-15). [T, [PR #16](https://github.com/inklikeurine/bifrost-htpc/pull/16): "the maintainer ran this branch on the real box and reported it working". Which box isn't recorded.]
- RPM Fusion often trails a Fedora update by a day or two, and anything built against both waits until it catches up. [B]
- How bifrost checks it: `vainfo --display drm`, looking for `VAProfileH264…VAEntrypointVLD` (and HEVC, VP9, AV1); in Firefox, `about:support` → Media → Codec Support. [B]

## Sound to the TV

- The HDMI/DisplayPort outputs share one sound card with the analog outputs, and WirePlumber (PipeWire's session manager) ranks analog higher, so a box with a built-in speaker plays through that. A WirePlumber rule (`device.profile.priority.rules` putting `output:hdmi-stereo` first) puts the TV first. [B, rule in `files/etc/wireplumber/`]
- The TV's output exists only while the screen is on: when the desktop turned the screen off after 10 minutes idle, sound fell back to the box's speaker and came back when the screen woke. [B]
- The TV identifies itself to the box: `/proc/asound/card0/eld*` shows the TV's name, with `eld_valid 1` while connected. [B]

## Controlling the TV without HDMI-CEC (bifrost-roku-bridge)

- The box controls a Roku TV over the home network with Roku's External Control Protocol (ECP): power, volume, mute and input switching. [T, every key confirmed on the TV by hand, 2026-10-19]
- It finds the TV with SSDP (`roku:ecp`) and wakes it with Wake-on-LAN, retrying for 20 s. [B]
- The TV needs two settings changed:
  - Settings > System > Advanced system settings > Control by mobile apps > Network access: **Enabled**. "Limited", the default since Roku OS 14.1, refuses key presses with HTTP 403 [B; [S](https://selorahomes.com/docs/how-to/integrations/tcl/tcl-roku-tv-integration-setup/)].
  - Settings > System > Power > Fast TV start: **On**, which keeps it reachable in standby [B; [S](https://forum.developer.roku.com/t/waking-a-roku-tv-using-wake-on-lan-over-wi-fi/11332)].
- Protocol reference: [Roku External Control Protocol](https://developer.roku.com/docs/developer-program/dev-tools/external-control-api.md) [C, cited by bifrost].
- How the remote's volume keys reach the TV: the bridge grabs only the input devices that have a mapped key (`EVIOCGRAB`) and passes their other keys on through a virtual device (uinput), so the rest of the remote works as before. The box itself never sees the mapped keys. [B: the design; its tests note that taking over a real remote on the box wasn't covered]
- Why not rebind the keys in Plasma: Plasma's global-shortcut service rewrites its own shortcut file while it runs. [B]
- A held volume key repeats only as fast as the TV answers; repeats that arrive while a request is running are dropped, so the volume stops when the key is let go. [B]
- The box never turns the TV on by itself, so a 04:30 reboot leaves the room dark. [B]

## The WeChip W1 air mouse

- Its power button and mouse-mode button send nothing the PC can see. The power button is infrared only. [B]
- It can learn the TV remote's power code (hold Power + OK until its light stays on, then press the TV remote's power button head to head), which turns the TV on and off with no box involved. [B; [S, manual](https://manuals.plus/asin/B07WRZNW6X)]

## Web apps in Firefox kiosk mode

- `/etc/firefox/policies/policies.json` is read by Fedora's Firefox: a signed, self-distributed extension installed from it and was active. [T, bifrost README "tested"]
- Prefs bifrost sets in every web-app profile, each because a prompt or page can't be dismissed with a remote [B, `firefox-user.js`]:
  - no first-run, welcome, default-browser or translation pop-ups
  - no restore-session prompt after a reboot (`browser.sessionstore.resume_from_crash` false)
  - autoplay allowed (`media.autoplay.default` 0)
  - notification requests refused (`permissions.default.desktop-notification` 2)
  - no offer to save passwords (`signon.rememberSignons` false) [T: [issue #4](https://github.com/inklikeurine/bifrost-htpc/issues/4), the prompt couldn't be closed with the remote]
  - no "press Esc to exit full screen" banner (`full-screen-api.warning.timeout` 0)
- `--class <name>` makes a kiosk window carry its own app ID, so each web app shows its own icon. [B]
- The system's on-screen keyboard (Plasma's) opened on top of Leanback for Dropout's own keyboard. [T, [issue #2](https://github.com/inklikeurine/bifrost-htpc/issues/2), open]

## Moonfin (the Jellyfin client the lead is considering)

- Not on Flathub. Released on GitHub as a `.flatpak` bundle (`Moonfin_Linux_v<version>.flatpak`, app ID `org.moonfin.linux`). [B]
- The bundle is built without an update source, so Flatpak never sees new versions; only the AppImage updates in place. [B; [C, build script](https://github.com/Moonfin-Client/Moonfin-Core/blob/main/build-linux.sh)]
- The bundle isn't signed; bifrost checks it against the checksum GitHub publishes for the release file. [B]
- No full-screen setting: it reopens the way it was last closed, first as a 1280×720 window. Bifrost forced it full screen with a KWin window rule. [B]
- It has a TV mode, switched on per box. It stores the server login through KWallet on Plasma. [B]

## Box behaviour

- **"Is something playing?":** bifrost's reboot waits while an idle inhibitor is held (`systemd-inhibit --list`, what video players take to keep the screen on), or while any logged-in user has an audio stream that isn't paused (`pactl list sink-inputs`, `Corked: no`). Video and game streams both play audio. It rechecks every 30 minutes and reboots anyway after 3 hours. [B, `bifrost-reboot`]
- **Suspend:** bifrost turned automatic suspend off on AC power, because "a sleeping box misses its update timers and may not wake for the remote". [B, comment in `setup.sh`]
- **After a power cut:** set the firmware's AC Recovery (or similar) to Power On, so the box comes back by itself. [B]
- **Notifications:** with no notification service, an app that shows one waits about 25 s for an answer. [B]

## Not confirmed

- Which box ran bifrost v0.1.0 when H.264 hardware decoding was reported working.
- The OptiPlex's CPU and RAM.
- The bridge taking over a real remote's keys on the box; Wake-on-LAN from deep standby.
- Anything about the LG TV's own network control: bifrost has nothing on it.
