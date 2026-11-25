# Test image results: image, updates, installer, profiles, keyring, DNS, codecs, Firefox, keyboard

*Claude, 2026-11-24, at the lead's request ("Test whatever you need to test"). A throwaway mkosi image of Fedora 45 (systemd 262.1, kernel 7.3.4) in a virtual machine with UEFI and a virtual TPM. No product code. Everything below was seen in the VM unless marked otherwise.*

## The short answer

The plan's choices hold up. The tests found traps that would have broken the product quietly; they're marked **trap**. Not covered in the VM yet: signed updates, SELinux enforcing, the recovery menu, rebuilding the codec add-on after an update, driver add-ons.

| Area | Result |
|---|---|
| Image | Boots on UEFI in 11–13 seconds in the VM. `/usr` is read-only and verified (dm-verity); the second slot and the data partition are made on first boot. 1.9 GB on disk |
| Updates | Version 1 to 2 in 40 seconds, into the other slot |
| Rollback | A deliberately broken version 3 failed three boots in about 90 seconds; the box went back to version 2 by itself, data kept |
| "Good" mark | Waits for the shell to report in, as designed |
| Installer | systemd's installer API installed to a blank disk in 75 seconds with progress; the disk booted by itself |
| Profiles | greetd sessions register with logind; switching ended the old profile's programs; `useradd` and `userdel` work with `/usr` read-only |
| Keyring | Opens with no prompt, after a reboot too; Flatpak apps get their secret |
| DNS filter | Per-profile filtering works, Flatpak apps included; AdGuard Home runs as its own user with only the port-53 right |
| Codec add-on | Built on the box in 20 seconds; adds H.264 and HEVC; a mismatched one is skipped |
| Firefox from outside | Policies, AutoConfig, a pre-installed signed extension and a system-wide native-messaging host all work from read-only `/usr` |
| Keyboard | Works in a GTK4 Flatpak, a Qt Flatpak and an SDL3 program |

## Image

- Packages a minimal Fedora image lacks and Hearth needs: `systemd-pam` (without it sessions never reach logind; **trap**), `systemd-container` (holds the updater's downloader; **trap**).
- mkosi calls: `UnifiedKernelImages=unsigned` until Secure Boot; `UnifiedKernelImageFormat=` must be set so the boot file's name matches what the updater looks for.
- `/etc` is copied from the image once, on first boot, so later images can't change it. Hearth's own settings live in `/usr` instead (agreed 2026-11-24).
- SELinux was set to log only. Enforcing it is untested.

## Updates and rollback

- **Trap:** the boot file's `Mode=0444`, which systemd's own example uses, sets the read-only flag on the boot partition. The boot loader then can't rename the file to count tries, so automatic rollback silently never happens. `Mode=0644` works. Tested both ways.
- After a rollback, the updater still lists the bad version as "current", because it's the newest installed. Hearth must read the running version from `/usr/lib/os-release`, and remember a version that failed.
- The test's health check used `FailureAction=reboot`, so each failed boot restarts the box. Without it, nothing was tested.
- Not tested: signed updates (signature checking was off).

## Installer

- `ListCandidateDevices` leaves out the stick it booted from, and reports size, fit and minimum size. It refuses normal users. `Run` gave 118 progress messages.
- **Trap:** the installer writes `default hearth-*` in the boot loader's settings, which matches its own entry (`hearth-commit_1.2`) but not a file called `hearth_3.efi`. Updates installed and were never booted. With the line removed, the update booted and rolled back correctly. So: name the files `hearth-…` with a dash, or have the Hearth page remove the line. Only the second was tested.
- The installer keeps the kernel in `/boot/hearth/`, so the updater calls the installed version "incomplete". Hearth's page should tidy this after installing.
- Not tested: the "Installer" boot profile with Hearth's page on screen (the page doesn't exist yet).

## Profiles and sessions

- The login screen's job works through greetd's own protocol: create a session, start it, no Unix password. It needs a PAM file of Hearth's own, since Hearth checks PINs itself.
- `loginctl terminate-user` ended labwc, Firefox, a Flatpak and a detached program in 0.6 seconds.
- A program started outside the session (from another login) survived, and `userdel` refused until it was killed. Hearth ends every program of a user before deleting it.
- A session that exits at once makes greetd loop quickly. The login screen needs a limit and a failure screen.
- `/var/spool/mail` and `/var/lib/greetd` don't exist in the image; `tmpfiles.d` should make them.

## Keyring (oo7)

- A random password sealed with `systemd-creds --uid`, placed in the profile's credential store, opened the keyring with no prompt. oo7 used 13 MB. The password isn't in the keyring file, and another user can't read or open it.
- oo7 isn't started by default in Fedora. The session has to enable it, or Flatpak apps get an error (**trap**).
- **Trap:** when the password is lost, or the TPM is wiped, oo7 reports "active" and simply finds nothing; saving fails. Hearth must check for itself. Moving the old keyring aside gives a working fresh one.
- It needs a portal file for the desktop name `Hearth` and a session target that pulls in `graphical-session.target`. With both, apps see the Secret, file chooser, screenshot, open-link, notification, settings, inhibit and screen-cast portals.

## DNS filter

- nftables gives each profile its own local address by Unix user, so AdGuard Home sees separate clients. A blocked site and forced safe search applied to the kid only. The kid couldn't get around it by asking another DNS server, and couldn't reach AdGuard's control page. AdGuard Home used 30 MB.
- **Trap:** with systemd-resolved running, lookups come from *its* user and skip the kid's rules. Without it (`/etc/resolv.conf` pointing at AdGuard, name lookups set to `dns`), host and Flatpak apps were both filtered. So: no systemd-resolved.
- IPv6 needs its own rules (blocking a kid's IPv6 DNS was tested).
- Not closed: an app that does its own encrypted DNS to a fixed address. Firefox's is locked off by policy.
- "No internet": with the connection cut, AdGuard answers "server failure" at once. The shell can use that with no new connection.

## Codec add-on

- RPM Fusion's `libavcodec-freeworld`, `intel-media-driver` and `mesa-va-drivers-freeworld` install over the image in a scratch copy; what they add becomes a system extension (107 MB). FFmpeg then had H.264 and HEVC. Only a cache file differed from the image's own.
- The extension must leave out the package database, or it hides the box's own.
- Needs `ldconfig` after loading. An extension tagged for another version was skipped, and the box carried on.
- Caveats: package signature checks were off, and the scratch copy lacked `/var/tmp`, so one package script failed. Real decoding needs hardware. Rebuilding after an update wasn't tested.

## Firefox from outside

- All from read-only `/usr`: `policies.json`, AutoConfig with the sandbox off, two signed extensions installed by policy from a file in the image, and a native-messaging host started from `/usr/lib64/mozilla/native-messaging-hosts`.
- DNS over HTTPS locked off, telemetry off, notifications blocked, site blocking and requested language all applied. The language didn't change because no language pack was installed.
- AutoConfig catches Backspace and the remote's Back. Escape doesn't reach it; labwc already takes it.
- **Local only:** with telemetry and studies off by policy, Firefox still fetched its experiments list (Nimbus) and installed a new-tab add-on by itself. Needs its own setting.
- Inconclusive: the right-click menu in kiosk mode. Page alerts open and close with Enter.

## Keyboard beyond Firefox

| App | Keyboard | Types | Full screen | Home works |
|---|---|---|---|---|
| GTK4 Flatpak | Shows | Yes | Yes | Yes |
| Qt Flatpak | Shows on a text page, stays shut on a page with no text box | Yes | Yes, but Qt can draw its own title bar | Yes |
| SDL3 program | Shows | Yes | Yes | Yes |

- One letter of "cats", sent with no gap at all, was lost in the GTK4 app; at 30 ms and 60 ms apart none were.
- The GTK4 app with unsaved text didn't close when Hearth opened another app, probably waiting to ask about saving. The shell needs a rule for that.

## Still needs real hardware

- labwc's Vulkan renderer, and so HDR. In the VM labwc refused to start with it, so Hearth must choose the renderer per box and fall back.
- Hardware video decode, TV colour range, sound, sleep and wake, RAM on a 4 GB box, overlays over playing video, NVIDIA and Wi-Fi drivers.
