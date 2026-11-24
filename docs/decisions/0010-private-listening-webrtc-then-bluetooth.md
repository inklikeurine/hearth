# Decision 0010: Private listening: a WebRTC phone page first, then Bluetooth

Related: design doc, section 7 (phone features, priority 2); [Optional hardware](0008-optional-hardware-supported-never-assumed.md); [private listening research](../research/2026-11-19-private-listening.md).

## Context

Private listening means the TV's sound in your headphones, like Roku's feature. The hard part is lip sync: every platform plays the audio with a known delay and holds the picture back by the same amount. Hearth's own video player (mpv) can do that exactly; Firefox can only follow the delay its audio output reports, which is untested for a network sink.

Boxes may have no Bluetooth radio. The lead has no iPhone to test with. This work is scoped for v2.0.0

## Decision

1. Build order: a WebRTC web page on the phone first, then Bluetooth. The phone page runs in any browser and needs no app and no radio on the box. It sits on an existing server, MediaMTX or go2rtc; the choice waits for the afternoon latency test in the research note.
2. Bluetooth is one path with two modes, classic (A2DP) and LE Audio, sharing BlueZ, PipeWire, the pairing screen and delay handling.
3. Detect and hide. Hearth checks for a radio, LE Audio support and a network, and shows only what works, with a one-line reason ("No Bluetooth adapter found").
4. Video delay: mpv's offset; Firefox follows the delay its audio output reports. If it doesn't, keep the total phone-path delay under about 100 ms. No Firefox delay hack.
5. Research on recommended adapters is deferred until the project is further along (milestone to be chosen by the lead).
6. iPhone stays designed for but unverified until a beta or contributor tester with one reports back.

## Consequences

- The phone page adds a named, switchable LAN port, off by default, with a pairing token; STUN stays off.
- The afternoon test (any Linux PC with PipeWire and an Android phone) decides MediaMTX vs go2rtc and shows whether Firefox follows a network sink's delay.
- Bluetooth work starts after the phone page, and only boxes with a radio get it.

## Turned down

- Bluetooth first: the smallest build, but useless on the many boxes without a radio.
- All paths at once: too much for one developer.
- roc-toolkit: lowest delay, but its Android app is a work in progress with no release since June 2024, and there's no iPhone app.
- Snapcast: built for multi-room sync, 1 s default delay, control port open to the LAN.
- PipeWire RTP: no phone client; multicast by default, which is unreliable over Wi-Fi.
- A Firefox video-delay hack: fragile and site-specific.
- Showing every option and failing with an error: worse than hiding what the box can't do.
