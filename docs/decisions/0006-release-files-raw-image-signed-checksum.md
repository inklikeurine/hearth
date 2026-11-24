# Decision 0006: Releases are a raw disk image in a .xz with a signed checksum; signing starts after alpha testing

Related: design doc, sections 2 and 5.2 ([OS image and updates](../design.md)); [Installer boot profile](0005-installer-boot-profile-sysinstall.md).

## Context

Users download one file and write it to a USB stick. The same files also feed the box's own updates. Research: [installer research](../research/2026-11-19-installer-flasher-and-usb-install.md).

- Raw or ISO. mkosi builds GPT disk images (raw); its listed outputs have no ISO (secondary source). Every app in the install guide writes the file byte for byte, so a raw image boots as it is. Rufus offers an "ISO mode" and a "DD mode" for hybrid ISOs and recommends ISO mode ([Rufus FAQ](https://github.com/pbatard/rufus/wiki/FAQ)); a raw image avoids that choice. Costs of raw: Windows shows the stick as unreadable or "needs formatting"; the stick's spare space is hidden until it's reformatted; on a larger stick the backup partition table lands mid-stick.
- Updates only check GPG. `systemd-sysupdate` checks each downloaded `SHA256SUMS` file against a detached `SHA256SUMS.gpg`, using the keyring `/usr/lib/systemd/import-pubring.pgp`. `Verify=` defaults to yes and "should be left enabled, outside of test environments" ([sysupdate.d](https://www.freedesktop.org/software/systemd/man/latest/sysupdate.d.html)). The page mentions no other signature type.

## Decision

- The release is one raw disk image, compressed as `.xz`, with a SHA-256 checksum of the `.xz`. Users check the file they downloaded; the flashing app unpacks it.
- The checksum file is signed. The lead holds the signing key, made through a GitHub Action.
- Signing method ("option 2"): a long-lived GPG key signs `SHA256SUMS`, plus a GitHub attestation as extra proof.
- All of it is implemented after alpha testing. Alpha images and checksums are unsigned.

## Consequences

Easier:

- One signed file serves both the download page and the box's updates.
- Raw means no ISO/DD choice for the user and nothing extra to build.

Harder:

- The wiki must warn about the "needs formatting" message on Windows and the hidden spare space.

## Not decided

- Where the key lives and where its backup goes; decide before the first signed image. Claude's proposal: protect it with branch protection and a protected environment that needs approval before the workflow can use it (not researched).

## Not confirmed

- Whether attestations need a paid GitHub plan.
- Offline checking.
- mkosi's current signing options.
- How the image's update keyring is filled at build time.
- Whether mkosi's ISO support in `systemd-repart` (v27 notes) is mature.

## Turned down

- ISO: gains nothing for the apps we picked; can be added alongside later.
- Attestation only: updates would fail the check, or `Verify=` would have to be off.
- GPG key only: works; the lead chose to add the attestation.
- Signing from the first alpha: would let alpha boxes update into beta without a reinstall. The lead chose to wait.
