# Hearth network allowlist: first draft (2026-11-19)

> FLAGGED: NEEDS REVIEW BY A STRONGER MODEL. The lead judged this task mishandled and asked for it to be redone at a higher level. See "Why this is flagged" below. Do not treat the table as the final allowlist.

Status: research draft. Repo not touched. No further research was done after the lead's answers.
Tags: C = confirmed in source or docs I read this session (links in Sources). S = secondary source. M = from memory, not confirmed. Host names tagged M need checking before they go in any repo file.

## Why this is flagged

- The lead's goal is to support users installing apps, make them work well, and tighten security. It is not to account for every app's network behaviour.
- This draft drifted into a per-app audit (Moonlight, Jellyfin, KDE Connect, and so on) and asked the lead questions about individual apps. Wrong scope.
- It used terms the lead hadn't been given (for example "captive-portal check") and asked nine questions, which the lead found unusable.
- Reviewer: redo this at platform level (see "What the reviewer should do").

## Decided by the lead (2026-11-19, in the lead's words, my reading in brackets)

| # | Topic | Lead's answer | My reading |
|---|---|---|---|
| 1 | AdGuard Home upstream DNS | "adguard defaults are fine." Also: an in-Hearth screen to change AdGuard settings "would be nice." | Keep AdGuard's shipped defaults (upstream Cloudflare, per the table). The settings screen is a wish, not scheduled work. |
| 2 | Time source | "whatever the base image default is." | Use Fedora's default time service. I did not confirm which one that is (chrony per memory, M). |
| 3 | Firefox | Support the latest Firefox release only; Hearth pins the version in the image (lead says 160+ today). "We can allow firefox through. We want to trust apps (mostly). This needs to be seamless for the end-user." | Firefox's connections are allowed and not locked down heavily. Remote Settings stays on. Old-version worries are dropped. Not stated: what happens to Firefox telemetry and studies. Ground rule says no telemetry, so I read them as still off by policy. Reviewer to confirm with the lead. |
| 4 | Firefox Safe Browsing | "Off." | Turn it off. |
| 5 | Firefox packaging | Lead's expectation: native Firefox in the OS image. Asked whether Fedora has a native Firefox release, or whether we build one. | In the image, not a Flatpak. Which source (Fedora's package or our own build) is open. |
| 6 | Individual apps | "User decided, not me. Stop focusing on individual apps." Job is to support users installing apps and help them function well, while tightening security. | Per-app rows are reference only. Which Jellyfin client to use is the user's choice. |
| 7 | Moonlight's launch-time calls | "it's fine." | Accept them as they are, no patch. (Vague: could also mean "block at network". I read it as accept.) |
| 8 | Research on remaining gaps | Not at this time. | No research prompts written. |

## Open, not decided

- Captive-portal check (the lead didn't know the term; I should have explained it). Hotel, café and airport wifi often blocks all internet until you accept terms on a web page. A "captive-portal check" is the device quietly fetching a tiny test page to see whether it's being blocked, so it can tell the user to log in. Both Fedora's NetworkManager and Firefox do it, to a third party, over plain HTTP. On a living-room TV box it matters mostly for travel or shared wifi. Decision still needed: on, off, or off with a switch. I had proposed off by default; the lead hasn't answered.
- Where Firefox comes from. The lead asks if Fedora has a native Firefox release. Memory (M, not checked here): Fedora ships a `firefox` package in its own repos, with Fedora's own default settings. How closely it follows Mozilla's release timing is unchecked. If it doesn't fit, building our own from Mozilla's release source is the alternative. Reviewer to check.
- Firefox telemetry and studies under "seamless": see decision 3.

## What the reviewer should do

Proposed by Claude, not approved by the lead:

1. Reframe the work at platform level. Two questions: what connections does the base system Hearth ships make (OS, updates, DNS, time, Firefox, AdGuard, KDE Connect, firmware), and what general mechanism lets users install any app while Hearth shows and limits what it connects to. For the second, Flatpak's sandbox network permission is the likely starting point (M); verify.
2. Keep the base-system rows below. Treat the third-party app rows (Moonlight, Jellyfin, Plasma Bigscreen web-app viewer) as reference only.
3. Settle the open items above with the lead in plain language, with a real example for each. Define every term the first time.
4. Fold in the lead's wish for an AdGuard settings screen as a possible later feature.
5. Keep the test-VM recording method (below); it still applies.

## Findings worth keeping

- Almost every outbound connection has an off switch or can be left out of the image. Two can't reach zero: DNS to some upstream and a clock source. OS updates need one named host too.
- Firefox Remote Settings can be switched off by policy on Firefox 153+ (C), so on the latest Firefox the switch exists. The lead's decision is to leave it on.
- Moonlight makes network calls at every launch with no setting to stop them (C). Lead accepted this.
- Defaults that would bypass AdGuard Home unless set: systemd-resolved's built-in fallback DNS servers (C), and Firefox's DNS-over-HTTPS rollout (C). AdGuard's own default upstream is Cloudflare (C).
- Possible conflict with ground rules: Plasma Bigscreen's `webapp-viewer` needs QtWebEngine, which is Chromium (C). Hearth's plan is Firefox kiosk instead.

## Allowlist table (reference)

Columns: component | destination | why | on by default? | how to turn off or repoint | needed for Hearth?

### Fedora base

| Component | Destination | Why | Default | Off / repoint | Needed? |
|---|---|---|---|---|---|
| NetworkManager connectivity check | `http://fedoraproject.org/static/hotspot.txt`, every 300 s (C) | Captive-portal / internet check (see "Open") | On only if package `NetworkManager-config-connectivity-fedora` is installed (C: the file ships in that package's config; whether Fedora installs it by default: M) | Don't ship the package, or `[connectivity] enabled=false` in `/etc/NetworkManager/conf.d/` (M) | Open |
| systemd-timesyncd | Google NTP servers compiled in (C); exact host names M | Set the clock | On if timesyncd is the time service | `NTP=` and `FallbackNTP=` in `timesyncd.conf.d/` (M) | Yes (some clock source) |
| chrony | Fedora pool servers in Fedora's default config (M) | Set the clock | On if chrony is shipped | `pool` / `server` lines in `/etc/chrony.conf` (M) | Yes (some clock source) |
| systemd-resolved fallback DNS | Cloudflare, Google, Quad9 compiled in (C) | Used when no DNS server is configured | On when resolved is used and no DNS is given | Set `FallbackDNS=` empty in `resolved.conf.d/` (M) | No — bypasses AdGuard |
| systemd-sysupdate | HTTPS host named in `url-file` / `url-tar` in `.transfer` files; checksums from `SHA256SUMS` (C) | OS updates (GitHub Releases or a mirror per the design doc, section 5.2) | Only if the timer is enabled; timer interval unconfirmed | Edit `[Source]` in the `.transfer` file; mask `systemd-sysupdate.timer` | Yes |
| dnf / rpm-ostree "countme" | Fedora mirror lists, with a counting flag (C: Fedora's documented opt-out strings) | Counts Fedora installs | Only where dnf or rpm-ostree runs | `countme=0` in repo files (C) | No — don't ship dnf on the box |
| fwupd | `https://cdn.fwupd.org/downloads/firmware.xml.<compression>` (C: `lvfs.conf`) | Firmware metadata | Upstream build default is on (C: meson option `lvfs` defaults `true`, writes `Enabled=true`); Fedora package setting M | `Enabled=false` in `/etc/fwupd/remotes.d/lvfs.conf` (C), or don't ship fwupd | Opt-in |
| Certificate updates | No network call; CA bundle comes with the package (M) | — | — | — | — |
| Flatpak / Flathub | `dl.flathub.org` (M; `.flatpakrepo` file not readable here) | Install and update app Flatpaks; sends header `Flatpak-Os-Info: os-id;version;arch` | `report-os-info` default true (C) | `report-os-info=false` in Flatpak config (C key; file path unconfirmed); remote URL via `flatpak remote-modify --url` (M) | Opt-in (Hearth's own parts aren't Flatpaks) |
| AppStream / ODRS | KDE Discover calls the ODRS ratings API, `odrs.gnome.org` (M host; call itself C) | App reviews | On if Discover is shipped | Don't ship Discover | No |
| fontconfig | None known (M; source not readable here) | — | — | — | — |

### Firefox (kiosk web apps)

Policy names below are confirmed to exist in `Policies.sys.mjs` and the schema. The `Preferences` policy only accepts prefs starting with `browser.`, `network.`, `dom.`, `media.`, `extensions.`, `app.update.` and a few others (C). It does not accept `services.settings.*` or `toolkit.telemetry.*`.

| Component | Destination | Why | Default | Off / repoint | Needed? |
|---|---|---|---|---|---|
| Remote Settings | `firefox.settings.services.mozilla.com` (M host) | Blocklists, certificate data, study recipes | On | Policy `DisableRemoteSettingsAndAcceptSecurityConsequences: true` (C, Firefox 153+ per schema) or env `MOZ_REMOTE_SETTINGS_DISABLED=1` (C). Server override is normally blocked on release (C) | Yes (security data). Lead: leave on |
| Telemetry / health report | `incoming.telemetry.mozilla.org` (M host) | Usage data | On in Mozilla builds (C: `datareporting.healthreport.uploadEnabled` and `datareporting.usage.uploadEnabled` true under `MOZ_DATA_REPORTING`) | Policy `DisableTelemetry` (C) | No (ground rule); lead hasn't confirmed under "seamless" |
| Studies (Normandy/Shield) | Via Remote Settings | Experiments | On | Policy `DisableFirefoxStudies` (C) | No (same note) |
| Region lookup | `location.services.mozilla.com/v1/country` (C) | Pick region defaults | On (C: `browser.region.update.enabled` true) | `browser.region.update.enabled=false` via `Preferences` policy (C: `browser.` allowed) | No |
| Captive-portal detection | `firefox-portal-detection.com`: `/generate_204` and `/success.txt?ipv4` (C) | See "Open" | On in Firefox (C: `firefox.js` sets `network.captive-portal-service.enabled` true) | Policy `CaptivePortal: false` (C) | Open |
| Safe Browsing | `safebrowsing.googleapis.com` (C: update URL) | Phishing/malware lists | On | No dedicated policy (C). Set `browser.safebrowsing.*` prefs via `Preferences` policy (prefix allowed C; exact pref names M) | Lead: off |
| Push | `dom.push.enabled` true (C); endpoint `push.services.mozilla.com` (M) | Web push | Pref on; connects only when a site subscribes (M) | `dom.push.enabled=false` via `Preferences` (C: `dom.` allowed) | No |
| DNS-over-HTTPS | Cloudflare, switched on by a Remote Settings rollout (C); canary `use-application-dns.net` (C) | Encrypted DNS | Rollout can switch it on | Policy `DNSOverHTTPS {Enabled:false, Locked:true}` (C). Firefox also skips the rollout if `network.trr.mode` or `network.trr.uri` has a user value (C) | No — would bypass AdGuard |
| Add-on updates | `versioncheck.addons.mozilla.org` (C) | Update add-ons | On | Policy `ExtensionUpdate: false` (C). Install our extension from the image via `ExtensionSettings` (C) | No |
| Firefox app updates | Mozilla update server (URL not in source; removed from prefs, C) | Update Firefox | `app.update.auto` true (C) | Policy `DisableAppUpdate` (C) | No — the image updates it |
| Widevine / OpenH264 (GMP) | `aus5.mozilla.org/update/3/GMP/…` (C), then vendor CDNs (M) | Download DRM / codec modules | Only when needed (M) | Policy `EncryptedMediaExtensions {Enabled:false}` (C); `media.gmp-manager.url` empty via `Preferences` (C: `media.` allowed) | Opt-in (design doc, section 8.4) |
| OCSP | Each certificate authority's responder (M) | Revocation checks | Default value not confirmed; pref `security.OCSP.enabled` exists and is allowed in `Preferences` (C) | `security.OCSP.enabled=0` | Check |
| Accounts, Suggest, New Tab content, search suggestions, prefetch, built-in AI features | Hosts M | Extras | On in a normal profile; kiosk-mode behaviour not tested | Policies `DisableFirefoxAccounts`, `FirefoxSuggest`, `FirefoxHome`, `SearchSuggestEnabled`, `NetworkPrediction`, `GenerativeAI` / `AIControls` (all C) | No |

### uBlock Origin

| Component | Destination | Why | Default | Off / repoint | Needed? |
|---|---|---|---|---|---|
| Filter-list updates | Hosts in `assets.json` (C): `cdn.jsdelivr.net`, `ublockorigin.github.io`, `ublockorigin.pages.dev`, `raw.githubusercontent.com`, `filters.adtidy.org`, `easylist-downloads.adblockplus.org`, plus ~12 more for optional lists. Default-on lists: uBlock filters (incl. badware, privacy, unbreak, quick-fixes), EasyList, EasyPrivacy, urlhaus-1, plowe-0 (C) | Keep lists current | `autoUpdate: true` (C: `background.js`) | uBO reads managed `userSettings` (C: `storage.js`), so `autoUpdate: false` should work there; not tested. Or ship lists in the image and block the hosts | Opt-in |

### Other Hearth components

| Component | Destination | Why | Default | Off / repoint | Needed? |
|---|---|---|---|---|---|
| AdGuard Home: filter lists | `adguardteam.github.io/HostlistsRegistry/assets/filter_1.txt` on, `filter_2.txt` off; update every 24 h (C: `config.go`) | Block lists | On | Change filter URLs or the update interval in `AdGuardHome.yaml` (key names M) | Yes |
| AdGuard Home: upstream DNS | `https://cloudflare-dns.com/dns-query`; bootstrap `1.0.0.1`, `1.1.1.1`, `2606:4700:4700::1001`, `2606:4700:4700::1111` (C) | Answer DNS lookups | On | `dns.upstream_dns` and `dns.bootstrap_dns` in `AdGuardHome.yaml` (key names M) | Yes. Lead: defaults fine |
| AdGuard Home: update check | `https://static.adtidy.org/adguardhome/<channel>/version.json` (C) | Tell the user a new version exists | On (M) | Command-line flag to skip the check (M — verify name) | No |
| AdGuard Home: Safe Browsing / parental hash lookups | `family.adguard-dns.com` (C, as found in source) | Optional filtering features | Default not confirmed | Leave the features off | Opt-in |
| KDE Connect | LAN only: UDP 1716 broadcast, TCP 1716–1764, mDNS, manual IPs (C for ports; mDNS service string unconfirmed) | Phone pairing | On while the daemon runs | Don't run `kdeconnectd`; firewall the ports. Manual IP entries can point anywhere. I didn't record an Internet host in the source; re-grep before locking this row | Opt-in (phone companion) |

### Third-party apps (reference only; not the goal, per the lead)

| Component | Destination | Why | Default | Off / repoint | Needed? |
|---|---|---|---|---|---|
| Jellyfin client | The user's own server; LAN discovery on UDP 7359 (C: Jellyfin networking docs) | Find and use the server | On when used | Enter the server address by hand | User's choice |
| Moonlight (moonlight-qt) | `moonlight-stream.org` for compatibility data and the controller mapping database, fetched at every launch (C: `computermanager.cpp`, `mappingmanager.cpp`); STUN lookup (host M); mDNS `_nvstream._tcp` on the LAN (C) | Game-streaming extras | On, no setting found to stop them | Block the hosts at the network, or patch | Lead: fine |
| Plasma Bigscreen `webapp-viewer` | QtWebEngine = Chromium (C). `kcms/webapps` fetches icons from `icons.duckduckgo.com` (C) | Web-app windows | — | Don't ship it; Hearth uses Firefox kiosk instead | No — conflicts with ground rules |
| Qt / KDE Frameworks startup | No hard-coded URLs in `qtbase/src/network` (C). `QNetworkInformation` reads NetworkManager over D-Bus, no Internet call (C) | — | — | — | — |

## Can't be turned off

- DNS upstream. AdGuard needs somewhere to send lookups. Zero is not possible.
- A clock source. TLS fails without a correct clock. Can be a LAN or router source, not zero.
- The OS update host. Only if Hearth updates over the network. Can be a mirror we control.
- Moonlight's launch-time fetches. No setting. Network block or patch only. Lead accepted this.
- Telemetry code in Firefox. Silenced by policy, not removed from the binary.

## How to verify (test VM, DNS recording)

DNS logs alone miss connections to raw IP addresses (AdGuard's `1.1.1.1` bootstrap, resolved's fallback DNS, NTP). Record both. Untested sketch:

1. Boot the built image in a VM on a private bridge (`br-hearth`). The host answers DHCP and DNS and forwards nothing to the Internet.
2. Log every DNS question on the host:
   ```
   dnsmasq --no-daemon --interface=br-hearth --bind-interfaces --no-resolv \
     --log-queries --log-facility=/tmp/hearth-dns.log \
     --dhcp-range=192.168.100.10,192.168.100.50
   ```
   Unanswered lookups are still logged, which is what we want.
3. Log every new outbound connection, including raw IPs:
   ```
   nft add table inet hearthtest
   nft add chain inet hearthtest fwd '{ type filter hook forward priority 0; policy drop; }'
   nft add rule inet hearthtest fwd ct state new log prefix \"HEARTH-EGRESS \" counter
   tcpdump -ni br-hearth -w egress.pcap 'not (udp port 67 or udp port 68)'
   ```
4. Run these scenarios, each from a fresh boot: first boot and 30 minutes idle; 25 hours idle (move the VM clock forward to trigger daily timers such as AdGuard's list update and fwupd); each Firefox web app opened; one OS update check; each opt-in feature turned on.
5. Pull the set of DNS names and destination IPs from the logs and compare to the allowlist file. Fail on any name or IP not in it. This is the egress test already named in the design doc, section 11.

## Gaps (couldn't confirm)

- Fedora package defaults: which time service, systemd fallback-DNS build settings, whether the NetworkManager connectivity package is installed by default, fwupd's Fedora `Enabled` flag and timer. The hosts that hold these (Fedora's package source, GitLab, pagure) were blocked from this workspace.
- Flathub's `.flatpakrepo` contents and the AppStream / media hosts. File is binary; `dl.flathub.org` refused.
- fontconfig: source unreachable; "no network" is from memory.
- systemd-sysupdate timer interval.
- Widevine download hosts after the Mozilla redirect.
- Firefox kiosk-mode startup traffic (New Tab, Suggest, Merino/Contile) and the default value of `security.OCSP.enabled`.
- Whether uBO's managed `autoUpdate: false` is honoured in practice.
- KDE Connect mDNS service string; KNewStuff provider URL; other KDE Frameworks startup calls.
- AdGuard Home config key names and CLI flag, and the default state of its Safe Browsing and parental features.
- Fedora's `firefox` package: how it tracks Mozilla releases and what defaults it sets (not checked).
- Everything tagged M needs a real boot with DNS capture.

## Sources

- NetworkManager Fedora connectivity config: https://github.com/NetworkManager/NetworkManager/blob/main/contrib/fedora/rpm/20-connectivity-fedora.conf
- Firefox prefs and policies: https://searchfox.org/mozilla-central/source/browser/app/profile/firefox.js · https://searchfox.org/mozilla-central/source/modules/libpref/init/all.js · https://searchfox.org/mozilla-central/source/modules/libpref/init/StaticPrefList.yaml · https://searchfox.org/mozilla-central/source/browser/components/enterprisepolicies/Policies.sys.mjs · https://searchfox.org/mozilla-central/source/browser/components/enterprisepolicies/schemas/policies-schema.json · https://searchfox.org/mozilla-central/source/services/settings/Utils.sys.mjs
- uBlock Origin: https://github.com/gorhill/uBlock (`assets/assets.json`, `src/js/background.js`, `src/js/storage.js`)
- AdGuard Home: https://github.com/AdguardTeam/AdGuardHome (`internal/home/config.go`)
- fwupd: https://github.com/fwupd/fwupd (`data/remotes.d/lvfs.conf`, `meson_options.txt`)
- Moonlight: https://github.com/moonlight-stream/moonlight-qt (`app/backend/computermanager.cpp`, `app/settings/mappingmanager.cpp`)
- Flatpak: https://github.com/flatpak/flatpak
- systemd: https://github.com/systemd/systemd · https://www.freedesktop.org/software/systemd/man/latest/sysupdate.d.html
- KDE Connect: https://github.com/KDE/kdeconnect-kde
- Plasma Bigscreen: https://github.com/KDE/plasma-bigscreen
- Jellyfin networking: https://jellyfin.org/docs/general/post-install/networking/
- Fedora countme opt-out strings: read in Fedora docs; URL not recorded; re-find before citing in the repo.
