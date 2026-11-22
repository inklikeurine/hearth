# AdGuard Home: getting it into the OS image

*Checked by Claude on 2026-11-22 for the lead's request to put the AdGuard Home import into the plan. Tags: [C] = read in the primary source linked, [S] = secondary, [M] = from memory.*

**Answer:** Fedora 45 doesn't package AdGuard Home, so the image build has to fetch it from AdGuard. AdGuard's own release files come with a checksum list, and the program inside is signed with AdGuard's OpenPGP key. Both checks work today. The program has a built-in self-updater, which Hearth switches off with one command-line option.

## Facts

**Packaging**
- Not in Fedora 45's own repos (searched the repo's copy of the package list, [f45-packages.txt](../f45-packages.txt)). Blocky, the lighter alternative, isn't either. [C]
- Licence: GPL-3.0. [C] ([GitHub API, repo licence](https://github.com/AdguardTeam/AdGuardHome))

**The current release**
- v1.0.0, published 2026-11-19. [C] ([release](https://github.com/AdguardTeam/AdGuardHome/releases/tag/v1.0.0))
- Files: one archive per platform (`AdGuardHome_linux_amd64.tar.gz` for Hearth), a separate web-interface archive, and `checksums.txt` (SHA-256 of every archive). No separate signature files. [C]
- The Linux archive holds the program `AdGuardHome`, `AdGuardHome.sig` (a signature over the program), `LICENSE.txt`, `CHANGELOG.md` and `README.md`. [C] (downloaded and listed, not run)
- `AdGuardHome_linux_amd64.tar.gz` v1.0.0: SHA-256 `19e023d98eb42eb1cdbfd6e4b6ebc937dade59e969506c62eafb3a3d6d2c1e4f`, the same as in `checksums.txt`. [C]
- The signature checks out: "Good signature from AdGuard <devteam@adguard.com>", key fingerprint `2864 5AC9 776E C4C0 0BCE 2AFC 0FE6 41E7 235E 2EC6` (RSA 4096, created 2022-08-15). [C]
- The key was fetched from [keys.openpgp.org](https://keys.openpgp.org/search?q=28645AC9776EC4C00BCE2AFC0FE641E7235E2EC6). That server publishes an email address on a key only after the address's owner confirms it [M]. A page on AdGuard's own site that gives this fingerprint: NOT FOUND (not searched yet).
- AdGuard's release script signs builds with GPG unless signing is switched off. [C] ([build-release.sh](https://github.com/AdguardTeam/AdGuardHome/blob/v1.0.0/scripts/make/build-release.sh))

**Versions after 1.0** ([CHANGELOG](https://github.com/AdguardTeam/AdGuardHome/blob/v1.0.0/CHANGELOG.md)) [C]
- Patch versions (1.0.x): bug fixes and security updates, "should mostly never break things". Minor versions (1.x): features; APIs may be removed after a deprecation. Security fixes may break compatibility in any release.
- v1.0.0 changed the default upstream DNS from Quad9 to Cloudflare. The [allowlist draft](2026-11-19-network-allowlist-first-draft.md) already lists Cloudflare, so the lead's "AdGuard's defaults are fine" was made on the current default.
- v1.0.1 is planned for about 2026-12-01.

**Command-line options that matter** ([options.go](https://github.com/AdguardTeam/AdGuardHome/blob/v1.0.0/internal/home/options.go)) [C]
- `--config` (path to the config file), `--work-dir` (working folder), `--web-addr` (where its web page and API listen, `host:port`), `--no-check-update` ("Don't check for updates"), `--pidfile`, `--logfile`, `--no-permcheck`.
- `--update` replaces its own program file. On Hearth's read-only system that fails anyway; it must never be offered.

**The self-updater**
- It reads `https://static.adtidy.org/adguardhome/<channel>/version.json`. [C] ([updater.go](https://github.com/AdguardTeam/AdGuardHome/blob/v1.0.0/internal/updater/updater.go#L73-L80))
- `--no-check-update` switches updates off before anything else is checked. [C] ([home.go](https://github.com/AdguardTeam/AdGuardHome/blob/v1.0.0/internal/home/home.go#L606-L634)). The allowlist draft had this flag's name as "from memory, verify": now confirmed.

**mkosi (the image builder)** ([mkosi man page](https://github.com/systemd/mkosi/blob/main/mkosi/resources/man/mkosi.1.md)) [C]
- `ExtraTrees=` copies a folder from the build machine into the image after the distro packages are installed.
- Build scripts run with no network unless `WithNetwork=` is set. Sync scripts (`mkosi.sync`) run before the build.

## From memory, to check

- Port 53 (DNS) needs root or the `CAP_NET_BIND_SERVICE` capability; a systemd unit can grant just that capability to an unprivileged user. [M]
- systemd-resolved listens on `127.0.0.53:53`. AdGuard Home on `127.0.0.1:53` doesn't clash with it, but on `0.0.0.0:53` it does. [M]
- Building from source needs Go plus Node.js and npm for the web interface. Both are packaged in Fedora 45 [C, package list]; the npm part pulls hundreds of packages from npm's registry [M].
- GPL-3.0: shipping the program means offering its matching source code for as long as the image is offered, so Hearth keeps the source archive of every version it ships. [M]

## Not confirmed

- An AdGuard-run page that publishes the signing key's fingerprint.
- That AdGuard Home runs as an unprivileged service on Fedora 45 next to systemd-resolved (spike).
- What the web-interface archive is for when the program already serves a web page (probably for builds without the built-in interface). [M]
