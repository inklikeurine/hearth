# Hearth install guide plan: writing the USB stick, and how the install to the internal disk works (updated 2026-11-19)

Labels: [C] confirmed in a primary source (linked). [S] secondary source only. [M] from memory or my own reasoning. [U] not confirmed.

## 1. What you decided
- The wiki (GitHub) has end-user guides for: Fedora Media Writer, Rufus (Windows), balenaEtcher (Windows and macOS), and GNOME Disks (Linux). Fedora Media Writer is the recommended one.
- A different app per platform is fine.
- Publish the image as `.xz` plus its checksum. The image inside is a raw disk image, not an ISO.
- Android is dropped.
- Scope: a guide that gets our final image onto a USB stick for installation. Hearth does not build or ship a flasher.
- Installer on the box: a boot profile that runs `systemd-sysinstall`, driven by a Hearth on-screen page (section 2).
- Fedora 45 as the base, not 44.
- Sign the checksum file. You hold the signing key, made through a GitHub Action.
- Signing setup: "option 2", all of it implemented after alpha testing. Option 2 = a long-lived GPG key signs `SHA256SUMS`, plus a GitHub attestation as extra proof (section 8). You confirmed my reading: alpha images and checksums are unsigned.
- Wiki writing is deferred until 1.0.0. Sections 3, 5 and 7 are the plan to pick up then.
- My reading, not yet confirmed by you: Raspberry Pi Imager is not in the guide, because you didn't list it.

## 2. Installer design (decided; reasons and risks kept here)
An "Installer" boot profile that runs `systemd-sysinstall`, driven by a Hearth on-screen page, not the stock text screen. Separate from writing the stick.
- Why: sysinstall is a terminal tool that asks questions with a keyboard [C]. A TV box has a remote. A Hearth page can ask "Install to this drive? Everything on it is erased", then call sysinstall with the answers filled in (the man page has a fully automatic example: `--erase=yes --confirm=no`) [C].
- Hard to undo: the partition layout (A/B slots, sizes) the installer writes. Fixing it on boxes already in homes is painful [M]. Fix it before the first public image.
- Risk: ParticleOS, the closest example, says it has no backwards-compatibility promise [C](https://github.com/systemd/particleos). sysinstall is new in systemd 261.

Fedora 45. sysinstall was added in systemd 261 [C](https://www.freedesktop.org/software/systemd/man/latest/systemd-sysinstall.html). Fedora 45 ships 261.2 stable and 262.1 in updates; Fedora 44 ships 259.8 [C](https://packages.fedoraproject.org/pkgs/systemd/systemd-libs/). I did not check whether Fedora 45 has been released.

Checksum file: a SHA-256 of the `.xz` file, signed, published next to it. Users check the file they downloaded; the flasher then unpacks it. mkosi can write the `.xz` and a `SHA256SUMS` file, optionally signed with gpg [S](https://manpath.be/f34/1/mkosi) (old man pages; current options not rechecked).

Image format: raw disk image. Reasons:
1. It is what mkosi builds: its listed outputs are GPT disk images, tar, cpio, unified images, sysext and directory trees, with no ISO [S](https://drss.io/reader/npub1v46qzhxhuh537gqp4qngvlfyza9zh9qvnyn08asa5zks54xvcm6setyhlt/01687c7a2456947dfc01ad33201225021ad19c9c2ccefe399a242083fda42431). The mkosi v27 notes mention new ISO-boot support in `systemd-repart` [S](https://newreleases.io/project/github/systemd/mkosi/release/v27); I did not check how finished it is.
2. Every app we picked writes the file byte for byte to the stick, so a raw image boots as it is. An ISO gains nothing there.
3. Rufus shows a choice between "ISO mode" and "DD mode" for hybrid ISOs and recommends ISO mode, which rebuilds the stick's contents instead of copying it [C](https://github.com/pbatard/rufus/wiki/FAQ). A non-technical user could pick wrong. A raw image gives Rufus no such choice [S](https://www.msftnext.com/?p=41172).
4. `systemd-sysinstall` copies the OS from the booted medium [C]. A stick with real partitions matches that [M].
- Costs: Windows shows the stick as unreadable or "needs formatting" afterwards [S]; the stick's spare space is hidden until reformatted (Media Writer has a Restore button [C]); the image's backup partition table lands mid-stick on a larger stick, which Rufus's FAQ lists as a raw-write drawback [C]. The wiki warns about the first two.
- Hard to undo: little. An ISO can be added later alongside.

## 3. Apps in the guide

| Platform | Recommended | Also documented |
|---|---|---|
| Windows | Fedora Media Writer | Rufus, balenaEtcher |
| macOS | Fedora Media Writer | balenaEtcher |
| Linux | Fedora Media Writer (Flathub) | GNOME Disks |

Fedora Media Writer [C]: Windows, macOS, Linux; Qt, GPL-2.0, not Electron. Writes the image straight to the stick, then reads it back in full to verify ([README](https://github.com/FedoraQt/MediaWriter)). "Custom image" button ([Fedora Magazine](https://fedoramagazine.org/?p=15119)). Latest 5.3.2, 13 Jul, year not shown ([releases](https://github.com/FedoraQt/MediaWriter/releases)). `xz-libs` is a build dependency on all three systems, so it reads `.xz`. Whether the file picker accepts our file name [U]. Sends a User-Agent string; rest of its privacy file not read [U].

Rufus (Windows only) [C]: [rufus.ie](https://rufus.ie/en/) lists Windows 8+ only, GPL-3.0+, version 4.16 dated 2026.11.17. Built-in checksum button (MD5, SHA-1, SHA-256) ([FAQ](https://github.com/pbatard/rufus/wiki/FAQ)); the only app here with one. Reads `.xz`, `.gz`, `.bz2`, `.lzma` compressed raw images [S](https://gitdab.com/nerd/rufus/raw/tag/v3.1/ChangeLog.txt). Update check is on without a prompt if the file is named `rufus.exe`; other names get a prompt [C]. The guide tells users to decline it. Does it pick raw mode by itself for our file? [U]

balenaEtcher (Windows, macOS) [C unless marked]: checks the write ([site](https://etcher.balena.io/)); 2.1.6, 13 May, 2026 by file dates ([releases](https://github.com/balena-io/etcher/releases)). Electron [S], over 400 MiB [S]. Analytics removed in 2.1.2; optional crash logs with a setting ([balena post](https://blog.balena.io/balenaetcher-and-privacy/)). Tails dropped it from its Windows instructions over privacy concerns in 2025 [S]. The wiki says it is an Electron app. Does it open `.xz`? [U]

GNOME Disks (Linux) [C]: right-click the file → Disk Image Writer, or menu → Restore Disk Image ([man page](https://man.archlinux.org/man/gnome-disks.1); [GNOME OS doc](https://os.gnome.org/docs/install.html)). `.xz` restore was added in 2013 ([commit](https://mail.gnome.org/archives/commits-list/2013-June/msg04619.html)), but an old issue still asks for compressed-image support [U]. No checksum feature found [U]. If it can't open `.xz`, the Linux page says "unpack first (`xz -d`)" or "use Media Writer".

Not in the guide: Raspberry Pi Imager (Pi device list first; statistics on by default [C]), Ventoy (no macOS [C]), EtchDroid (Android dropped).

## 4. How systemd-style images install from USB

Two patterns exist:
1. Write the whole disk image to the internal disk, then let `systemd-repart` grow it on first boot. No installer. Works for a developer; gives a non-technical person no way to pick a drive. [M]
2. Boot an installer medium that copies the OS onto the target disk. What ParticleOS and GNOME OS do.

ParticleOS [C](https://github.com/systemd/particleos): build with mkosi, write the stick with `mkosi burn /dev/<usb>`, boot it, choose the "Installer" boot profile. That runs `systemd-sysinstall`: asks for the target drive, partitions it, copies the OS, sets up the boot partition (`bootctl install`), installs the kernel (`bootctl link`). Reboot into the internal disk. Updates later by `systemd-sysupdate`.

What `systemd-sysinstall` does [C](https://www.freedesktop.org/software/systemd/man/latest/systemd-sysinstall.html): ask for the disk → check size → erase or add to free space → ask about registering in the firmware boot menu → summary and confirm → encrypt locale, keyboard and timezone settings as credentials (locked to the TPM if there is one) → `systemd-repart` using definitions in `/usr/lib/repart.sysinstall.d/` (falls back to `/usr/lib/repart.d/`) → `bootctl link` → `bootctl install` → reboot. The man page says it reboots, but `--reboot=` defaults to false; test what happens.

GNOME OS [C](https://os.gnome.org/docs/install.html): users download an ISO and write it with GNOME Disks. They boot it with UEFI and Secure Boot turned off, create a user, click "Install now", pick the disk, press Install. The installer is built on `systemd-repart`; the live image uses the same `/usr` partition as the installed OS, and install can run without rebooting ([2025 talk](https://app.media.ccc.de/v/all-systems-go-2025-352-gnome-os-prt--booter-image), [2024 talk](https://app.media.ccc.de/v/all-systems-go-2024-283-installing-your-os-with-systemd-repart)). It copies partition images, not one disk image [S](https://inbox.spectrum-os.org/spectrum-devel/87pla0prky.fsf@alyssa.is).

## 5. Wiki plan (deferred until 1.0.0)

Pages: Install overview (what you need, the whole flow in pictures), Windows, macOS, Linux, Boot menu keys by maker, Troubleshooting. Each OS page has Fedora Media Writer first, then the other apps with only the pictures that differ. Screens below come from Fedora's docs and may differ in 5.3.2; every picture needs capture on a real machine [U].

Shared pictures (overview page)
1. Download page: the `.xz` file name, size, the checksum file and its signature.
2. "What you need": USB stick (size TBD; image size not measured), a computer, the box, the TV. Red note: the stick will be erased.
3. Browser with the download finished; file name circled. Note: do not unpack the `.xz`.
4. Optional: check the download. Windows `Get-FileHash` (or Rufus's checksum button), Mac `shasum -a 256`, Linux `sha256sum`; screenshot with the matching text highlighted.

Windows page
5. Download Media Writer; installer; first-run window. If a "missing .dll" error appears, install Microsoft's Visual C++ Redistributable [C, Fedora docs]. SmartScreen warning [U].
6. Main screen: click Custom image.
7. File dialog with the Hearth file selected.
8. Stick selected; red Write to disk button.
9. Progress, then finished. Eject the stick. Windows may say the stick "needs formatting": click Cancel [M].
- *Rufus:* download, SELECT the file, pick the stick, START; decline the update check.
- *balenaEtcher:* Flash from file, Select target, Flash. It's an Electron app.

macOS page
5. Download the `.dmg`, drag to Applications. Older Fedora docs say Ctrl-click → Open to get past the security prompt [C, old docs; today's behaviour U]. Then pictures 6–9.
- *balenaEtcher:* same three steps as Windows.

Linux page
5. Install Media Writer from Flathub (Fedora's recommended route), or `sudo dnf install mediawriter` on Fedora [C]. Then pictures 6–9.
- *GNOME Disks:* right-click the file → Disk Image Writer; pick the stick; Start Restoring.

On the box (overview page, same for every path)
10. Stick into the box, TV on, power on.
11. The boot menu: key differs by maker (lookup table needed; Dell is F12 [M]).
12. Boot menu list with the USB entry circled (may show as "UEFI: …").
13. Hearth installer page: internal drives with size and name; one pre-selected if only one.
14. "Everything on <drive> will be erased" with Install / Cancel.
15. Progress bar.
16. "Remove the USB stick", then restart.
17. First start: Hearth welcome.
18. "It didn't start from the stick": wrong port, Secure Boot on, no UEFI entry.

## 6. Conflicts with the ground rules
- Electron: only Etcher. Allowed for users. The wiki says so.
- Local only: concerns Hearth, not the user's PC. The wiki still warns: Rufus checks for updates by default if named `rufus.exe`, and Media Writer fetches Fedora's release list at start [M]. None touches the box.
- Secure Boot: GNOME OS needs it off [C]. If 1.0 does the same, the user has to change a firmware setting. That is the hardest step for a non-technical person, and no flasher helps. This sits uneasily with "security first".
- AI policy: I propose no contribution to Fedora, systemd, ParticleOS or any flasher.

## 7. Not confirmed / needs real hardware
- Do all four apps open our `.xz` as downloaded? Media Writer (file picker names), Rufus (raw mode chosen by itself), Etcher (`.xz` unconfirmed), GNOME Disks (`.xz` unconfirmed). One test per app.
- Does Media Writer check a published hash, or contact the network when you pick a local file?
- Windows SmartScreen and macOS security prompts on today's Media Writer, Rufus and Etcher builds.
- The same image written on Windows, macOS and Linux, booting on the OptiPlex.
- `systemd-sysinstall` on our image: reboot behaviour, running it unattended from a Hearth page, no TPM, Fedora 45's version with our partition definitions.
- Boot-menu keys per maker; firmware that hides USB entries.
- Image size and the smallest stick that works.
- Media Writer's privacy file; GNOME Disks release date.
- Signing: GitHub's plan requirements for attestations; offline verification; current mkosi signing options; how the image's update keyring is filled at build time.

## 8. Signing

Decided by you: option 2, all of it implemented after alpha testing. Option 2 = a long-lived GPG key signs `SHA256SUMS`, plus a GitHub attestation as extra proof. Alpha images and checksums are unsigned (you confirmed this reading).

Consequence of that decision [M, follows from the sysupdate finding below]: `systemd-sysupdate` verifies updates with the public key inside the installed image. Boxes installed from an unsigned alpha have no such key, so they can't verify later signed updates. Alpha boxes will need a reinstall to get onto signed updates, or alpha builds run with `Verify=` off, which the docs say is for test environments only. Not tested.

Why option 2 and not C alone: our update tool only checks GPG. `systemd-sysupdate` checks each downloaded `SHA256SUMS` file against a detached `SHA256SUMS.gpg`, using the keyring `/usr/lib/systemd/import-pubring.pgp` (plus `/etc/systemd/import-pubring.pgp`). `Verify=` defaults to yes and "should be left enabled, outside of test environments" [C](https://www.freedesktop.org/software/systemd/man/latest/sysupdate.d.html). The page mentions no other signature type; Sigstore support is not confirmed [U]. So updates need a long-lived GPG key, and its public half ships inside every image.

What the attestation half is [C](https://docs.github.com/en/actions/concepts/security/artifact-attestations): GitHub attestations are signed claims about how software was built (workflow, repository, commit, trigger), signed through Sigstore. In public repos they go into a public, permanent transparency log; in private repos they use GitHub's own Sigstore instance with no log. They are "not a guarantee that an artifact is secure"; they only help if someone verifies them. Generating one needs `id-token: write`, `attestations: write` and `contents: read` in the workflow, plus the `actions/attest` step [C](https://docs.github.com/en/actions/how-tos/secure-your-work/use-artifact-attestations/use-artifact-attestations). Verifying uses the GitHub CLI (`gh attestation verify <file> -R <owner>/<repo>`), and the docs assume an online machine. The wiki would not ask non-technical users to do this. Plan requirements and offline use are not confirmed [U].

Claude proposed, not decided:
- Custody: protect the GPG key with branch protection and a protected environment with required approval before the workflow can use it [M, not researched]. Anyone who can edit the workflow or read the secret can sign.
- A fresh key per release does not work: installed boxes would reject it [M].
- Hard to undo: the key. The first public image fixes our trust root. If the key leaks or is lost, a new key has to arrive in an update signed by the old one, or users must reinstall [M]. Decide custody and a backup before the first signed image.
