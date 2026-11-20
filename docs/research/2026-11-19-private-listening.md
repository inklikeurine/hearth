# Private listening: research (2026-11-19)

Status: research summary, closed out 2026-11-19. All decisions from this chat are recorded below. Repo not touched; decision records not yet written.
Tags: C = confirmed in a source I read this session (link in Sources). S = secondary source (news, forum, third-party site). M = from memory or my own inference. T = needs a test on real hardware.

## Outcome

- Every platform does the same three things: play the audio somewhere with a known, fixed delay; delay the picture by the same amount; let the user trim by hand. Roku also measures the delay with the phone's camera and microphone.
- mpv can delay video exactly. Firefox has no video-delay control. It only follows the delay its audio output reports. Whether that works for a network sink is untested (T).
- Bluetooth headphones paired to the box is the cheapest path: PipeWire gets the headphones' reported delay and players can use it. It needs a Bluetooth radio, which the box may not have.
- Phone over Wi-Fi: no option is drop-in. A WebRTC web page needs no app, works on iPhone and needs no radio on the box, but is the most to build. roc-toolkit has the lowest delay, but its Android app is a work-in-progress and has had no release since June 2024, and I found no iPhone app. Snapcast is active but built for multi-room sync; its default delay is 1 s.
- Biggest unknown: the real delay on your Wi-Fi. The afternoon test at the end measures it.

## Decided by the lead (2026-11-19)

1. Hardware. Boxes may ship without Bluetooth. The project has no control over end hardware and assumes old low-end laptops and mini-PCs. Hearth assumes nothing beyond the minimum target specs. A "recommended adapters" section is allowed; a hardware requirement is not. (Lead's words: "Boxes may ship without bluetooth. This project has no control over end hardware. Assume old low-end laptops and mini-PC's. We can include a 'recommended adapters' section, but the project should overall assume that we don't know what we're being installed on (outside of our minimum target specs).")
2. Three paths. Design for a Bluetooth path, a Bluetooth LE path and a WebRTC path. (Lead: "We may need a bluetooth, bluetooth le, and webrtc path.") Later confirmed by "we'll go with your recommendations".
3. Accepted Claude's recommendations (lead: "we'll go with your recomendations"). Each shows what was turned down:
   - Build order: WebRTC first, then Bluetooth. Turned down: Bluetooth first; all at once.
   - Phone-over-Wi-Fi method: a WebRTC web page on an existing server (MediaMTX or go2rtc). The choice between the two waits for the afternoon test. Turned down: roc-toolkit, Snapcast, PipeWire RTP as the base.
   - Video delay: mpv offset, with Firefox following the delay its audio output reports. If Firefox doesn't follow, fall back to keeping total phone-path delay under about 100 ms. Turned down: building a Firefox delay hack.
   - Bluetooth is one path with two modes (classic A2DP and LE Audio), not two separate paths. They share BlueZ, PipeWire, the pairing screen and delay handling.
   - Detect and hide. Hearth checks at startup whether a radio, LE Audio support and a network are present, and shows only what works, with a one-line reason ("No Bluetooth adapter found"). Turned down: showing every option and failing with an error.
   - Recommended-adapters research is deferred until the project is further along (the lead will pick the milestone). Turned down: writing the list now from forum claims.
4. iPhone. The lead has no iPhone to test with. Testing waits for beta or contributor testing. (Lead: "I don't have an iphone to test with. That'll have to wait till beta testing/contributor testing.") Claude's reading: the web page stays designed for iPhone, since it runs in any browser and costs nothing extra to build; iPhone is not verified until a tester with one reports back. Not read as "iPhone is out of v1". Lead to correct if wrong.

## Open questions for the lead

None from this chat.

## Notes behind the decisions: no wireless on the box, unknown hardware

- Bluetooth-to-box needs a Bluetooth radio, often a USB adapter. Hearth cannot count on a radio being there.
- Low-delay Bluetooth (LE Audio) needs an adapter whose chip supports isochronous channels. Also: Linux kernel 6.4 or later, BlueZ with its experimental ISO sockets switched on in `main.conf`, and headphones that speak LE Audio too (C: [Collabora, Nov 2025](https://www.collabora.com/news-and-blog/blog/2025/11/24/implementing-bluetooth-le-audio-and-auracast-on-linux-systems/)). Check an adapter in `bluetoothctl` → `menu mgmt`: `cis-central` and `cis-peripheral` for headphones, `iso-broadcaster` and `sync-receiver` for broadcast (C: Collabora).
- Old laptops and mini-PCs will mostly lack it. Some chips can't do LE Audio, and never will: Ezurio's BT850 and BT860 (C: [Ezurio FAQ](https://www.ezurio.com/support/faqs/the-bluez-ble-stack-now-offers-le-audio-support-can-the-bt850-or-bt860-support-le-audio-with-bluez)); Intel's AX200 (S: [Intel forum](https://community.intel.com/t5/Wireless/Intel-Bluetooth-Isochronous-channels-and-other-mandatory/m-p/1595248), quoting an Intel Linux developer). The newer AX210 reportedly does. One vendor sells an LE Audio dongle (Feasycom FSC-BP409), unverified on Linux (S: [vendor page](https://www.feasycom.com/fsc-bp409); T). Radios in older machines are usually classic Bluetooth or early LE (M, unconfirmed). Expect classic Bluetooth (SBC, about 170–250 ms) to be the common case and LE Audio the exception.
- Classic delay is still fine for lip sync if the headphones report their delay and the player honours it (M, T). It is poor for games and Moonlight.
- The Wi-Fi phone path does not need a radio on the box. The box sits on Ethernet (or its own Wi-Fi); the phone is the wireless end. It works as long as the router lets Wi-Fi phones talk to wired devices (M). Things that break it: "client isolation", guest networks, mesh systems that split wired and Wi-Fi devices onto separate subnets, mDNS not crossing between them (M). Multicast over Wi-Fi is unreliable (M), another reason to skip PipeWire RTP's default.

## 1. How Roku and others do it

| Platform | How the sound reaches the listener | Lip sync |
|---|---|---|
| Roku | Audio goes over the home Wi-Fi to the free Roku phone app (iOS/Android); headphones plug into or pair with the phone. Up to 4 phones (C: [Roku support](https://support.roku.com/en-gb/article/217479488)). The wire protocol isn't published (M; a Roku forum poster says the same). | Turning it on mid-playback restarts the video so it can sync with the headphone audio (C). Manual "Adjust audio delay" wizard plus a slider (C). Newer: the phone points at the TV and uses camera and microphone to measure the drift, then applies a correction (S: [Cord Cutters News, Sept 2026](https://cordcuttersnews.com/rokus-bluetooth-wireless-audio-gets-a-built-in-sync-tool-so-your-audio-matches-the-lips-on-the-tv/)). Users report some Roku sticks need it every time and Roku TVs don't (S: [forum](https://community.roku.com/t5/Roku-mobile-app/Private-listening-needs-syncing-every-time/m-p/970376)). |
| Apple TV | Bluetooth headphones/AirPods straight to the box, or AirPlay 2 speakers (C: [Apple](https://support.apple.com/HT210526)) | Plays tones through the wired speakers to measure the TV's audio/video delay, then matches the wireless speakers (C: [Apple](https://support.apple.com/guide/tv/atvb228b7711/tvos)) |
| Google TV / Android TV | Bluetooth headphones straight to the TV (C: [Sony](https://www.sony.ca/en/electronics/support/audio-video-accessories-headphone-accessories/articles/00159290)). I did not confirm whether a phone-streaming mode exists. | Manual "A/V sync" setting on the TV (C: Sony) |
| Fire TV | Bluetooth straight to the box, no phone app (S: [The Streamable](https://thestreamable.com/how-to-use-private-listening-mode-on-your-roku-or-fire-tv)) | Not checked |

Two standards help: Bluetooth A2DP "delay reporting" lets headphones tell the player their delay (S: [Arch forum](https://bbs.archlinux.org/viewtopic.php?id=287046)), and Bluetooth LE Audio carries a "presentation delay" (S: [Novel Bits](https://novelbits.io/bluetooth-le-audio-auracast-profiles/)).

How much delay people notice. ITU-R BT.1359-1: detectable when audio leads by more than 45 ms or lags by more than 125 ms. Broadcast rule EBU R37: audio no more than 40 ms early or 60 ms late, end to end (S: [Wikipedia summary](https://en.wikipedia.org/wiki/Audio-to-video_synchronization)). Audio up to roughly 100 ms late is barely noticed; 150 ms and up is (S: [SoundGuys](https://www.soundguys.com/android-bluetooth-latency-22732/)).

## 2. Linux options for PipeWire audio to a phone

| | roc-toolkit | Snapcast | PipeWire RTP | WebRTC to a phone browser |
|---|---|---|---|---|
| Expected delay | Author measured about 7 ms on wired Ethernet (12 ms with USB sound cards); Wi-Fi not tested (C: [author's article](https://gavv.net/articles/roc-0.3/)). Receiver latency is a setting (C: [PipeWire manpage](https://manpages.debian.org/trixie/libpipewire-0.3-modules/libpipewire-module-roc-source.7.en.html)). Wi-Fi: T. | Server `buffer` setting is the capture-to-playback delay; default 1000 ms (C: [snapserver.conf](https://github.com/snapcast/snapcast/blob/master/server/etc/snapserver.conf)). Opus codec is the low-latency one; FLAC adds about 26 ms (C: [config doc](https://sources.debian.org/src/snapcast/0.34.0-2/doc/configuration.md)). Lowest workable buffer: T. | Receiver latency default 100 ms in the session module; packet size 2–20 ms (C: [rtp-session](https://manpages.opensuse.org/Leap-15.6/pipewire-modules-0_3/libpipewire-module-rtp-session.7.en.html), [rtp-sink](https://manpages.ubuntu.com/manpages/resolute/man7/libpipewire-module-rtp-sink.7.html)). Default is multicast, which is often poor on Wi-Fi (M). | No source found. GStreamer talk measured two-way WebRTC audio at about 230 ms, down from 650–700 ms (S: [GStreamer Conference 2025](https://gstconf.ubicast.tv/permalink/v126b0f19cf26yzfb7p5/)). Page can set `jitterBufferTarget`: Chrome 124+, Firefox 115+, not Safari/iOS (C: [MDN](https://developer.mozilla.org/en-US/docs/Web/API/RTCRtpReceiver/jitterBufferTarget)). Real figure: T. |
| Is the delay fixed and known? | Yes, receiver holds a target latency (C: [rocd README](https://github.com/roc-streaming/rocd)) | Yes. Every client plays each chunk exactly `buffer` ms after capture (C) | Roughly | No. The browser decides unless you set the target; Safari ignores it |
| Phone app | Android: Roc Droid, v0.2.2, June 2024, "work in progress" (C: [F-Droid](https://f-droid.org/en/packages/org.rocstreaming.rocdroid/)). iPhone: none found (T). | Android: Snapdroid on F-Droid (C: [F-Droid](https://f-droid.org/wiki/page/de.badaix.snapcast)). Snapweb page is "optimized for mobile", but the page doesn't say whether it plays audio (C: [Snapweb](https://github.com/badaix/snapweb)). iPhone: none found (T). | None found for phones (T). VLC can open an SDP file (M, untested). | None. Any browser, scan a QR code |
| Maintenance | Toolkit: latest release v0.4.0, June 2024, nothing newer on the releases page (C: [releases](https://github.com/roc-streaming/roc-toolkit/releases)). PipeWire ships the sink/source modules itself (C); Fedora packages them as `pipewire-module-roc` (C: [Fedora](https://packages.fedoraproject.org/pkgs/pipewire/)). Commit activity: not checked. | Active: v0.35.0 on 10 March 2026, seven releases since August (C: [releases](https://github.com/snapcast/snapcast/releases)) | Part of PipeWire, documented through 1.7 (C) | MediaMTX and go2rtc are both widely used (M); release dates not checked. WHEP, the playback protocol, is still an unfinished IETF draft (S: [dev.to](https://dev.to/masonwritescode/ship-a-go-live-button-browser-whip-publishing-with-mediamtx-no-sdks-4kfp)) |
| What it opens on the home network | Unicast UDP from the box to the phone: three ports on the phone (examples use 10001–10003). No encryption or login mentioned in the protocol docs (C: [protocols](https://roc-streaming.org/toolkit/docs/internals/network_protocols.html)). The box needs the phone's IP, so a pairing step is needed. | Box listens on TCP 1704 (stream), 1705 (control), 1780 (web/WebSocket); optional TLS on 1788 with client certificates (C: snapserver.conf, [config doc](https://github.com/snapcast/snapcast/blob/master/doc/configuration.md)). I found no username/password option (T). Anyone on the LAN can use the control port unless client certificates are on. | UDP to the chosen address, default multicast 224.0.0.56 (C). The session module announces itself over mDNS (C). No login or encryption mentioned. | An HTTP port plus a UDP port. MediaMTX defaults: 8889 and 8189/udp, no TLS, STUN off (C: [mediamtx.yml](https://github.com/bluenviron/mediamtx/blob/main/mediamtx.yml)). go2rtc: STUN is opt-in per its README (C: [go2rtc](https://github.com/AlexxIT/go2rtc/blob/HEAD/internal/webrtc/README.md)). Media is encrypted by WebRTC itself (M). You must add your own pairing token. |
| Verdict | Best delay, weakest phone story | Best sync guarantee, wrong tool (multi-room) | Skip: no phone client | Best reach, most to build; iOS lock screen is the weak spot |

WebRTC on iPhone. iOS Safari suspends WebRTC and Web Audio when the screen locks (S: [Apple forums](https://developer.apple.com/forums/thread/774239)). An older WebKit report says WebRTC fed straight into an `<audio>` element survives app switching, unlike Web Audio (S: [WebKit 215964](https://auto-bugs.webkit.org/show_bug.cgi?id=215964), 2020). Today's behaviour and Android Chrome's: T. The lead has no iPhone, so this waits for a beta or contributor tester with one (decision 4).

Wired box. All four options only need the box on the network; the phone is the wireless end. The router must let Wi-Fi phones reach wired devices (M, T on the lead's network).

Roku-style output on the box. A private-listening feed means the box's own speakers (the TV) go silent, so Hearth needs a separate PipeWire sink for it. Not tested.

Old CPUs. WebRTC needs the box to encode Opus in real time. Opus is light (M), but CPU and RAM on a 4 GB floor box is untested (T).

## 3. Delaying video to match the audio

(a) mpv: realistic.
- `--audio-delay=<sec>`: positive delays audio, negative delays video (C: [mpv options](https://github.com/mpv-player/mpv/blob/master/DOCS/man/options.rst)). Phone audio arrives late, so use a negative value, e.g. `--audio-delay=-0.15` delays the picture 150 ms. Default keys Ctrl+ and Ctrl- change it by 0.1 s (C: mpv man page).
- mpv's PipeWire output already adds the delay PipeWire reports for the stream (C: [`ao_pipewire.c`](https://github.com/mpv-player/mpv/blob/master/audio/out/ao_pipewire.c)); the PulseAudio output reads sink latency (C: `ao_pulse.c`). So a sink that reports its true delay needs no offset. A network sink can't know the phone's delay, so Hearth sets it: offset = network buffer + phone output delay (M), plus a user trim slider.
- Changing it while playing through libmpv: should work (M, untested).

(b) Firefox with YouTube/Twitch: not realistic as a built feature. One thing may make it unnecessary.
- I found no Firefox setting or web API to offset picture against sound (M).
- Firefox's audio clock follows the output device's *play cursor*, not what it has written, so on "Bluetooth or a remoted backend" where delay "can be hundreds" of ms the video is held back to match (C: [Firefox AudioSinkWrapper docs](https://firefox-source-docs.mozilla.org/media/AudioSinkWrapper.html)). So if the private-listening sink reports its real delay to PipeWire, Firefox should delay the video by itself. Whether PipeWire passes a virtual or network sink's delay to Firefox: T. This is the test that matters.
- WirePlumber's `node.latency-offset-msec` is documented for Bluetooth nodes (C: [WirePlumber](https://pipewire.pages.freedesktop.org/wireplumber/daemon/configuration/bluetooth.html)); the PipeWire change that added it says it adjusts the delay reported by the clock (C: [MR 460](https://gitlab.freedesktop.org/pipewire/pipewire/-/merge_requests/460)). Not confirmed for non-Bluetooth sinks.
- If Firefox does not follow: a Firefox hack would mean capturing the page's video into a buffer and drawing it late (extension), or capturing the whole window and re-showing it late (compositor). Both are heavy on a 4 GB box, and neither works for DRM video (M, unverified). Decided fallback: keep the total phone-path delay under about 100 ms, which sits inside the "audio late" detection limit.

## 4. Bluetooth headphones paired to the box

- Needs hardware the box may not have (see notes above).
- Works. PipeWire's Bluetooth codec list: SBC, SBC-XQ, AAC, LDAC, aptX, aptX HD, aptX LL, FastStream, LC3plus, Opus variants, LC3 (C: [WirePlumber docs](https://tamborim.pages.freedesktop.org/wireplumber/daemon/configuration/bluetooth.html)). PipeWire supports LE Audio's BAP in all roles; WirePlumber turns it on by default; needs Bluetooth 5.2+ on both ends (C: Collabora). LE Audio also needs an isochronous-capable controller, kernel 6.4+ and BlueZ ISO sockets enabled (C: Collabora). Real LE Audio delay with headphones on this stack: not measured (T).
- Delay is the problem. Classic Bluetooth with SBC measured 173 ms and 248 ms on two headphones; aptX Low Latency 34 ms but both ends must support it (S: [RTINGS](https://www.rtings.com/headphones/learn/latency), [SoundStage](https://www.soundstagesolo.com/index.php/features/185-latency-a-new-concern-for-audiophiles)). SoundGuys averaged about 308–369 ms across codecs on four Android phones (S: [dev.to summary](https://dev.to/keyboard-testerclick/your-bluetooth-headphones-are-300-ms-late-measure-it-in-one-browser-tab-2idp)).
- Lip sync is mostly automatic. PipeWire logs the delay the headphones report, e.g. "delay:300 ms" (C: [PipeWire issue 2902](https://gitlab.freedesktop.org/pipewire/pipewire/-/issues/2902)). Players use it to delay video (S). Catches: many headsets don't report a delay (one reporter: 1 of 4; C: MR 460), a 2023 bug kept the first reported delay from being applied (C: issue 2902; fixed? T), and manual offsets are ignored in some setups (S: [issue 3888](https://gitlab.freedesktop.org/pipewire/pipewire/-/issues/3888)).
- Fedora. The aptX plugin is not in Fedora; it comes from RPM Fusion as `pipewire-codec-aptx` (C: [RPM Fusion](https://download1.rpmfusion.org/free/fedora/development/rawhide/Everything/aarch64/os/repoview/pipewire-codec-aptx.html)). The LDAC library is in Fedora (C: [Fedora Packages](https://packages.fedoraproject.org/pkgs/libldac/libldac)). Which codecs Fedora's own PipeWire build includes: T (check `ls /usr/lib64/spa-0.2/bluez5/` on the image).
- Network exposure: none on the home LAN. One listener only (Roku allows four phones).

## Recommended adapters (deferred)

Deferred by the lead on 2026-11-19 until the project is further along (milestone to be chosen). Draft criteria for whoever picks it up: Bluetooth 5.2 or later; passes the `menu mgmt` isochronous check; driver in the mainline kernel; no firmware the image can't ship. Known so far: BT850 and BT860 fail; AX200 fails; AX210 reportedly passes (S); FSC-BP409 claimed (vendor, unverified). Nothing here is a recommendation.

## Order of the paths (decided)

1. WebRTC page on the phone, on top of MediaMTX or go2rtc. Works on any box with a network connection, no app, iPhone supported in design (untested). Needs: pairing token, a named and switchable port, Opus audio from a PipeWire sink.
2. Bluetooth headphones paired to the box, classic and LE Audio as one path with two modes. Smallest build and no ports, but only when the box has a radio. Detect and hide when it doesn't.
- Not in the plan: roc-toolkit (use it only for the afternoon test), Snapcast (multi-room design, 1 s default, open control port), PipeWire RTP (no phone client).

## Afternoon proof of concept

Goal: measure the real delay on your Wi-Fi, see if mpv fixes it, see if Firefox fixes itself. Any Linux PC with PipeWire and an Android phone. Untested on real hardware; the clip command was run here and the flash and beep line up. The PC can be on Ethernet to mirror a radio-less box. The numbers also decide MediaMTX versus go2rtc.

1. Test clip (white flash plus 1 kHz beep for 0.1 s every second):
```
ffmpeg -f lavfi -i "color=c=black:s=640x360:r=30:d=10,geq=lum='if(lt(mod(T,1),0.1),235,16)':cb=128:cr=128" \
  -f lavfi -i "aevalsrc='sin(2*PI*1000*t)*lt(mod(t,1),0.1)':s=48000:d=10" \
  -c:v libx264 -pix_fmt yuv420p -c:a aac -shortest sync-test.mp4
ffmpeg -i sync-test.mp4 -c:v libvpx-vp9 -b:v 1M -c:a libopus sync-test.webm   # for Firefox
```

2. Phone side: install Roc Droid from F-Droid, open Receive, note the phone's IP and the ports and settings it shows (must match the box).

3. Box side: install the module (Fedora: `sudo dnf install pipewire-module-roc`; Arch: `pipewire-roc`). Create `~/.config/pipewire/pipewire.conf.d/roc-sink.conf` (format from PipeWire docs, C):
```
context.modules = [
  { name = libpipewire-module-roc-sink
    args = {
      fec.code = disable
      remote.ip = PHONE_IP
      remote.source.port = 10001
      remote.repair.port = 10002
      remote.control.port = 10003
      sink.name = "Phone Sink"
      sink.props = { node.name = "phone-sink" }
      audio.position = [ FL FR ]
    }
  }
]
```
Then `systemctl --user restart pipewire pipewire-pulse wireplumber` and `pactl set-default-sink phone-sink`. If there's no sound, the phone app's FEC setting probably differs (T).

4. mpv: add to `~/.config/mpv/input.conf` (10 ms steps):
```
F5 add audio-delay -0.010
F6 add audio-delay 0.010
F7 show-text "audio-delay ${audio-delay}"
```
Run `mpv --loop sync-test.mp4` with headphones on the phone. Press F5 until the flash and the beep in the headphones land together. Press F7 for the number. That number is your total phone-path delay (a negative value of 0.17 means 170 ms). Leave it 10 minutes with the phone screen off, then copy a large file over the same Wi-Fi and watch for drift or dropouts. Also check the router: phone on Wi-Fi, PC on Ethernet, and confirm the stream works (client isolation test).

5. Firefox: open `sync-test.webm` in Firefox with the same sink and no offset. Same lag as mpv at zero offset means Firefox ignores the sink's delay. Flash and beep together means it followed. Also run `pactl list sinks` and read the Latency lines for `phone-sink`.

6. Bluetooth (30 min, only if you have a radio): run `bluetoothctl`, then `menu mgmt`, then `info`, and note whether `cis-central`, `cis-peripheral` appear. Pair headphones, make them the default sink, play the clip in mpv and Firefox with no offset. In sync means delay reporting works. Check `journalctl --user -u pipewire -b | grep -i "A2DP codec"` for the reported delay (log line seen in an older version; shows at default level? T). Note which codec was picked (SBC vs LC3).

Record: delay in ms on Wi-Fi (idle, loaded), drift after 10 minutes, dropouts, router lets phone reach the wired PC yes/no, Firefox yes/no, Bluetooth codec and yes/no per player.

## Conflicts with the ground rules

- Local only, every connection named and switchable: each Wi-Fi option adds new LAN ports. All must be off by default, listed in the audit screen, and have a switch. WebRTC servers must keep STUN off.
- Security first: Roc and Snapcast have no login. Snapcast's control port is open to the LAN. Needs a firewall rule limited to the paired phone, or a pairing token (WebRTC).
- No Chromium or Electron: none of the options needs it. The phone's browser is the user's own.
- Upstream AI policy: using these as tools needs no policy check. A patch, bug report or fork of Roc Droid, Snapcast or the others does. I checked none.
- Hardware floor: no conflict. The lead's decision keeps radios out of the requirements.

## Could not confirm / needs hardware

- Real Wi-Fi delay for roc-toolkit, Snapcast, WebRTC; lowest workable Snapcast `buffer`.
- Whether PipeWire passes a virtual or network sink's delay to Firefox (and `node.latency-offset-msec` outside Bluetooth).
- Whether Firefox follows Bluetooth delay reports on this stack.
- Any iPhone client for Roc or Snapcast; whether Snapweb plays audio in the browser.
- Anything on iPhone (lock-screen audio, `jitterBufferTarget` missing in Safari, real delay): needs a beta or contributor tester with an iPhone.
- Snapcast password login; Snapcast and Roc commit activity (GitHub API was blocked here); MediaMTX and go2rtc release dates.
- iOS Safari and Android Chrome behaviour with the screen locked, today.
- How Roku measures (beyond camera and microphone) and how it holds video back; the protocol is not published.
- Which Bluetooth codecs Fedora's own PipeWire build includes; the 2023 delay-not-applied bug status.
- Whether the Fedora-based test box has Bluetooth hardware (the OptiPlex 3080 Micro's radio status is unknown from the repo).
- What Bluetooth versions and chips old low-end laptops and mini-PCs carry (M only).
- Which cheap USB Bluetooth adapters pass the LE Audio check on Linux; the Feasycom dongle claim; real LE Audio delay with headphones on this stack. (Deferred.)
- Whether most home routers block Wi-Fi-to-wired traffic by default (M: usually not, but guest networks and some mesh setups do).
- 4 GB RAM and CPU cost of any option, including real-time Opus encoding on an old CPU.

## Sources

- Roku: https://support.roku.com/en-gb/article/217479488 · https://cordcuttersnews.com/rokus-bluetooth-wireless-audio-gets-a-built-in-sync-tool-so-your-audio-matches-the-lips-on-the-tv/ · https://community.roku.com/t5/Roku-mobile-app/Private-listening-needs-syncing-every-time/m-p/970376
- Apple: https://support.apple.com/guide/tv/atvb228b7711/tvos · https://support.apple.com/HT210526
- Sony Google TV: https://www.sony.ca/en/electronics/support/audio-video-accessories-headphone-accessories/articles/00159290
- Sync limits: https://en.wikipedia.org/wiki/Audio-to-video_synchronization · https://www.soundguys.com/android-bluetooth-latency-22732/
- roc-toolkit: https://gavv.net/articles/roc-0.3/ · https://roc-streaming.org/toolkit/docs/internals/network_protocols.html · https://github.com/roc-streaming/roc-toolkit/releases · https://github.com/roc-streaming/rocd · https://f-droid.org/en/packages/org.rocstreaming.rocdroid/ · https://manpages.debian.org/trixie/libpipewire-0.3-modules/libpipewire-module-roc-source.7.en.html · https://manpages.ubuntu.com/manpages/resolute/man7/libpipewire-module-roc-sink.7.html
- Snapcast: https://github.com/snapcast/snapcast/releases · https://github.com/snapcast/snapcast/blob/master/server/etc/snapserver.conf · https://github.com/snapcast/snapcast/blob/master/doc/configuration.md · https://github.com/badaix/snapweb · https://f-droid.org/wiki/page/de.badaix.snapcast
- PipeWire RTP: https://docs.pipewire.org/devel/page_module_rtp_sink.html · https://manpages.opensuse.org/Leap-15.6/pipewire-modules-0_3/libpipewire-module-rtp-session.7.en.html
- WebRTC: https://developer.mozilla.org/en-US/docs/Web/API/RTCRtpReceiver/jitterBufferTarget · https://github.com/bluenviron/mediamtx/blob/main/mediamtx.yml · https://github.com/AlexxIT/go2rtc/blob/HEAD/internal/webrtc/README.md · https://gstconf.ubicast.tv/permalink/v126b0f19cf26yzfb7p5/ · https://developer.apple.com/forums/thread/774239 · https://auto-bugs.webkit.org/show_bug.cgi?id=215964
- mpv: https://github.com/mpv-player/mpv/blob/master/DOCS/man/options.rst · https://github.com/mpv-player/mpv/blob/master/audio/out/ao_pipewire.c
- Firefox: https://firefox-source-docs.mozilla.org/media/AudioSinkWrapper.html
- Bluetooth: https://tamborim.pages.freedesktop.org/wireplumber/daemon/configuration/bluetooth.html · https://gitlab.freedesktop.org/pipewire/pipewire/-/merge_requests/460 · https://gitlab.freedesktop.org/pipewire/pipewire/-/issues/2902 · https://gitlab.freedesktop.org/pipewire/pipewire/-/issues/3888 · https://www.collabora.com/news-and-blog/blog/2025/11/24/implementing-bluetooth-le-audio-and-auracast-on-linux-systems/ · https://www.rtings.com/headphones/learn/latency · https://www.soundstagesolo.com/index.php/features/185-latency-a-new-concern-for-audiophiles · https://download1.rpmfusion.org/free/fedora/development/rawhide/Everything/aarch64/os/repoview/pipewire-codec-aptx.html · https://packages.fedoraproject.org/pkgs/libldac/libldac · https://packages.fedoraproject.org/pkgs/pipewire/
- Bluetooth adapters / LE Audio: https://www.ezurio.com/support/faqs/the-bluez-ble-stack-now-offers-le-audio-support-can-the-bt850-or-bt860-support-le-audio-with-bluez · https://community.intel.com/t5/Wireless/Intel-Bluetooth-Isochronous-channels-and-other-mandatory/m-p/1595248 · https://www.feasycom.com/fsc-bp409 · https://bluez.github.io/news/2022/le-audio-support/
