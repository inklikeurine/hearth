# Decision 0006: Releases are a raw disk image in a .xz with a signed checksum; signing starts after alpha testing

Accepted 2026-11-18 by the lead. Related: design doc, sections 2 and 5.2 ([OS image and updates](../design.md)); [Installer boot profile](0005-installer-boot-profile-sysinstall.md).

## Context

Users download one file and write it to a USB stick. The same signed files also feed the box's own updates. Research: [installer research](../research/2026-11-19-installer-flasher-and-usb-install.md).

- Raw or ISO. mkosi builds GPT disk images (raw); its listed outputs have no ISO (secondary source). Every app in the install guide writes the file byte for byte, so a raw image boots as it is. Rufus offers an "ISO mode" and a "DD mode" for hybrid ISOs and recommends ISO mode ([Rufus FAQ](https://github.com/pbatard/rufus/wiki/FAQ)); a raw image avoids that choice. Costs of raw: Windows shows the stick as unreadable or "needs formatting"; the stick's spare space is hidden until it's reformatted; on a larger stick the backup partition table lands mid-stick.
- Updates only check GPG. `systemd-sysupdate` checks each downloaded `SHA256SUMS` file against a detached `SHA256SUMS.gpg`, using the keyring `/usr/lib/systemd/import-pubring.pgp`. `Verify=` defaults to yes and "should be left enabled, outside of test environments" ([sysupdate.d](https://www.freedesktop.org/software/systemd/man/latest/sysupdate.d.html)). The page mentions no other signature type; Sigstore support isn't confirmed.
- GitHub artifact attestations are signed records of which workflow, repo and commit built a file, signed through Sigstore ([GitHub docs](https://docs.github.com/en/actions/concepts/security/artifact-attestations)). They prove where a file came from, not that it's safe. Checking one needs the `gh` command-line tool and a network connection.

## Decision

- The release is one raw disk image (not an ISO), compressed as `.xz`, with a SHA-256 checksum of the `.xz`. Users check the file they downloaded; the flashing app unpacks it.
- The checksum file is signed. The lead holds the signing key, made through a GitHub Action.
- Signing method ("option 2"): a long-lived GPG key signs `SHA256SUMS`, plus a GitHub attestation as extra proof.
- All of it is implemented after alpha testing. Alpha images and checksums are unsigned.

## Consequences

Easier:

- One signed file serves both the download page and the box's updates.
- Raw means no ISO/DD choice for the user and nothing extra to build.

Harder:

- Boxes installed from an unsigned alpha have no public key in the image, so they can't verify later signed updates. They need a reinstall to join signed updates, or alpha builds run with `Verify=` off (the docs say that's for test environments only). Not tested.
- The key is hard to undo. The first signed image fixes the trust root. If the key leaks or is lost, a new key has to arrive in an update signed by the old one, or users reinstall. A fresh key per release wouldn't work: installed boxes would reject it.
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
