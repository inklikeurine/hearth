# Secret store for a box with autologin and no desktop: oo7 vs gnome-keyring vs KeePassXC

*Research summary, 2026-11-19. Everything under "Claude's proposal" is still a proposal unless it appears under "Decisions by the lead".*

> Read with the later decisions. This research assumed one shared `hearth` user that logs in by itself. Since then each profile is its own Unix user ([One Unix user per profile](../decisions/0002-one-unix-user-per-profile.md)), and each profile's keyring unlocks from its own sealed random password made at profile creation ([Boot shows the profile picker](../decisions/0004-boot-shows-profile-picker.md)). The oo7 findings, the trade-offs and the test list still apply; the setup steps need redoing per profile.

Tags: [primary] read in official docs or source code today. [source] read in the project's code (cloned today, not run). [secondary] forum, wiki or issue thread. [memory] not re-checked today.

## Decisions by the lead
- oo7 is accepted as the secret store (said in the lead's own words: "oo7 is accepted for the keyring"). Recorded as [Secret store: oo7](../decisions/0001-keyring-oo7.md).
- Decided later: how it unlocks (see the note above). Still open: failure recovery and a "Saved logins" settings page (open questions in the lead's log).

## Claude's proposal
Use oo7 (oo7-daemon + oo7-portal). *(Accepted.)* Unlock it at every boot with a random per-box password, sealed with `systemd-creds` and handed to the daemon as a systemd credential. Don't use an empty password. *(Unlock method: still a proposal.)*

| | Options | Pick | Hard to undo? |
|---|---|---|---|
| Secret store | 1. oo7 with sealed password. 2. gnome-keyring with an empty password. 3. KeePassXC. | 1 | Low before 1.0. Switching later means every Flatpak app loses the key to its own saved logins and users sign in again. |
| How it unlocks | a. Sealed random password (TPM2 if present, plus host key). b. Empty password (file stored unencrypted). | a | Low. Same effect as above. |

## Comparison

| | oo7 | gnome-keyring | KeePassXC |
|---|---|---|---|
| Unlocks on autologin with no prompt | Yes, via a systemd credential. The daemon's own unit already imports `oo7.keyring-encryption-password` [source]. A boot test of this path worked in another distro's image [secondary]. | Only with an empty password, which stores the keyring unencrypted. No credential mechanism. PAM unlock needs the typed password, which autologin never supplies. [primary/secondary] | No clean way. A database must be opened in the GUI by someone (one collection per opened database) [source]. Scripted unlock is possible but undocumented. [memory] |
| What protects the secrets | Sealed random password. See the trade-off section. | Nothing (file permissions only). | The database password or key file, which must be supplied somehow. |
| RAM | oo7-daemon: 8.9 MB, peak 10.4 MB, one sample on Fedora 45 [secondary]. oo7-portal is a second small process, not measured. | Not measured. Small C daemon, a few MB [memory]. | Not measured. Full Qt GUI app, much larger [memory]. |
| Flatpak secret portal | Yes. oo7-portal implements the backend. It stores each app's key inside the oo7 daemon's default keyring, so the daemon must be unlocked [source]. Needs a config file (below). | Yes, since gnome-keyring 3.35.1 [primary]. Its portal file is limited to `UseIn=gnome;sway` [secondary]. | No. No portal code in its source (commit cd42236, 2026-11-17) [source]. |
| Maintenance | Very active. Latest commit 2026-11-17; fix merged 2026-11-09 [source]. Releases: 0.6.0 (Feb 2026), 0.7.0 alpha (Jul), 0.7.0 beta in Fedora 45. No 0.7.0 final seen. Fedora 45 Change: default Secret Service provider. GNOME OS ships it. | Alive but winding down. 50.0 shipped with GNOME 50. Written in C. Backers of oo7 say it lacks maintenance [secondary]. Fedora is swapping it out. I could not read its GitLab (blocked), so commit activity is unchecked. | Very active. 2.8.0-beta1 on 2026-09-23; commit 2026-11-17 [source]. |

Sources:
- oo7 daemon README: https://github.com/linux-credentials/oo7/blob/main/server/README.md
- oo7 repo: https://github.com/linux-credentials/oo7 (service file `server/data/oo7-daemon.service.in`, portal `portal/src/main.rs`, `portal/data/oo7-portal.portal.in`)
- Fedora 45 Change: https://fedoraproject.org/wiki/Changes/oo7_Secrets_Service_Provider
- Fedora packages: https://packages.fedoraproject.org/pkgs/oo7/oo7-daemon/ and https://packages.fedoraproject.org/pkgs/oo7/oo7-portal/
- RAM sample: https://discussion.fedoraproject.org/t/fedora-45-installation-issue-oo7-secrets-service-provider/202541
- Working boot test (secondary): https://github.com/starlit-os/krytis/issues/911
- gnome-keyring empty-password advice: https://wiki.gnome.org/Projects/GnomeKeyring/StoringSecrets and https://wiki.archlinux.org/title/GNOME_Keyring
- Secret portal: https://flatpak.github.io/xdg-desktop-portal/docs/doc-org.freedesktop.portal.Secret.html
- KeePassXC Secret Service plugin: `src/fdosecrets/README.md` in https://github.com/keepassxreboot/keepassxc
- systemd-creds: https://www.freedesktop.org/software/systemd/man/latest/systemd-creds.html

## Security trade-off
A box that logs in by itself has no secret a person types, so the keyring password has to live on the box. Nothing makes that fully safe.

- Empty password (gnome-keyring's only route): secrets sit on disk unencrypted. Anyone who copies the file reads them.
- Sealed random password (oo7):
  - Stops: someone who gets only the keyring file (a backup, an exported settings bundle, a support bundle).
  - With a TPM2: also stops someone who removes the disk and reads it in another machine. `systemd-creds` picks TPM2 automatically when present, and by default binds to no PCRs [primary].
  - Without a TPM2: the host key (`/var/lib/systemd/credential.secret`, root-only) is on the same disk, so a stolen disk can be decrypted. It's a speed bump.
  - Does not stop: anyone who boots another OS on the same box (a TPM key bound to no PCRs unseals for any code on that machine; PCR 7 binding means little while Secure Boot is off by default), or any process running as the `hearth` user. Per-user credentials decrypt for any process of that user [secondary, systemd 258 notes].
  - Flatpak apps without direct bus access only get their own per-app key through the portal, so they can't read each other's logins [memory].
- Cheap boxes may have no TPM2. The design must work without one.
- Firefox saves logins in its own profile files (`logins.json` + `key4.db`) [secondary, Mozilla support], not in the Secret Service. So this store does not cover the web apps' sessions. I did not check whether current Firefox uses libsecret for anything else.
- The control that covers the whole disk is LUKS with TPM auto-unlock, an optional extra. The keyring password is a cheap extra layer, not a vault.
- A TPM cleared by a BIOS reset makes the sealed password undecryptable. The keyring is then lost, along with every Flatpak app's key to its own logins. Needs a reset path (below).
- systemd 262 pins TPM2 credentials to the SRK, and they won't open on older systemd. Fedora 45 ships 262.1, Fedora 44 ships 259 [primary]. Rolling back to an older base release would break them.

## Setup steps for the mkosi image (draft, nothing built or run)
1. Base: Fedora 45. The oo7 daemon is packaged only for Rawhide and 45 (44 has no daemon). Fedora 45 has `0.7.0~beta^1.e91b98a`, built from a commit before the empty-credential fix, so don't rely on an empty password.
2. `[Content] Packages=oo7-daemon, oo7-portal, xdg-desktop-portal` (add `oo7-cli` for testing). Leave out `gnome-keyring` and check nothing pulls it in; only one program can own `org.freedesktop.secrets`.
3. Portal config in `mkosi.extra/usr/share/xdg-desktop-portal/portals.conf` (location [memory]; the oo7 README uses `/etc/xdg-desktop-portal/portals.conf` or `~/.config/xdg-desktop-portal/<desktop>-portals.conf`):
   ```
   [preferred]
   org.freedesktop.impl.portal.Secret=oo7-portal;
   ```
   Needed because oo7's `.portal` file says `UseIn=gnome` and xdg-desktop-portal ignores it otherwise [source].
4. Session must start `graphical-session.target`. The oo7-portal unit has `Requisite=graphical-session.target` [source]. Our session target has to bring that target up.
5. First-boot unit (system service, runs once, before the autologin getty, after `/home/hearth` exists). Skips if the credential file exists:
   ```
   install -d -m 0700 -o hearth -g hearth /home/hearth/.config/credstore.encrypted
   head -c 32 /dev/urandom | base64 -w0 | systemd-creds encrypt --uid=hearth \
     --name=oo7.keyring-encryption-password - \
     /home/hearth/.config/credstore.encrypted/oo7.keyring-encryption-password
   chown hearth:hearth /home/hearth/.config/credstore.encrypted/oo7.keyring-encryption-password
   ```
   Never run this at image build time: every box would share one key. It needs systemd 258+ for the user credential store [primary].
6. Enable units in `mkosi.postinst.chroot` (runs inside the image [primary]): `systemctl enable hearth-keyring-init.service` and `systemctl --global enable oo7-daemon.service` (the `--global` form is [memory]). The daemon starts by D-Bus on first use anyway.
7. Don't add the oo7 PAM module to the autologin PAM stack. With no typed password it has nothing to hand over.
8. Persistence: `/home` and `/var/lib/systemd` must survive A/B updates. Exclude `~/.local/share/keyrings` and `~/.config/credstore.encrypted` from the settings bundle and support bundle.
9. Reset path (to design): if the credential can't be decrypted, move the keyring aside, make a fresh credential, tell the user to sign in again.

## Test on real hardware (not confirmed)
- First boot: `systemctl --user status oo7-daemon` shows an unlocked default keyring, no prompt, no stalled app. Also test boot 2 and a reboot after an update.
- A Flatpak app that saves a login (use `oo7-cli` or a test app) works, and the login is still there after reboot.
- Check `systemd-creds.socket` is enabled in the image (user credentials decrypt through it, per systemd 258 notes [secondary]).
- RAM: measure oo7-daemon, oo7-portal and (for comparison) gnome-keyring-daemon with `/proc/<pid>/smaps_rollup`.
- Locked keyring with no desktop: the daemon picks a CLI prompter for non-graphical callers and a GNOME prompter for graphical ones [source]. Neither exists on Hearth, so a locked keyring means failed or stuck logins. Test what an app sees.
- Cleared or changed TPM: what does the unit and the daemon do?
- No-TPM machine (an older mini PC): confirm `systemd-creds` falls back to the host key and everything still unlocks.
- Empty credential on current main: the code I read turns it into "no secret" and keeps found keyrings locked [source, not run]. Not needed for the proposal.

## Known oo7 rough edges
Most open bugs are about migrating an old gnome-keyring or PAM-unlocked setup (#577, #651, #688), and one fresh-install report on Fedora 45 Workstation. A fresh Hearth image has no old keyring, so these mostly don't apply. Still beta, and the daemon has had alpha warnings.

## Ground-rule checks
- Local only: oo7-daemon's unit sets `PrivateNetwork=yes` [source]. No network use.
- Chromium/Electron: none. oo7 is Rust, KeePassXC is Qt.
- Hearth parts in the image, never Flatpak: oo7 daemon and portal are image packages. Fits.
- Upstream AI policy: I found no contributing or AI policy file in the oo7 repo. Per our rule, assume they don't want AI-assisted input. Hearth only uses oo7; file nothing upstream unless it's a critical flaw.
