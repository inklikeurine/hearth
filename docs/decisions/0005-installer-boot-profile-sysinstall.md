# Decision 0005: The box installs from an "Installer" boot profile, driven by a Hearth page

Related: design doc, section 2 ([OS image and updates](../design.md)); [Release files](0006-release-files-raw-image-signed-checksum.md); [Boot shows the profile picker](0004-boot-shows-profile-picker.md).

## Context

Hearth ships one disk image. The user writes it to a USB stick, boots the box from the stick, and installs to the box's internal drive. Hearth builds no flasher of its own.

Findings from the [installer research](../research/2026-11-19-installer-flasher-and-usb-install.md):

- `systemd-sysinstall` is a systemd tool for this step. It asks for the target disk, checks its size, erases it or uses free space, partitions it with `systemd-repart`, copies the OS, and sets up the boot partition. Added in systemd 261 ([man page](https://www.freedesktop.org/software/systemd/man/latest/systemd-sysinstall.html)).
- Fedora 45 ships systemd 261.2 (262.1 in updates) ([Fedora packages](https://packages.fedoraproject.org/pkgs/systemd/systemd-libs/)).
- sysinstall is a keyboard-driven text program, and a TV box has a remote. Its man page has a fully automatic mode (`--erase=yes --confirm=no`), so a program can fill in the answers.

## Decision

- The image has an "Installer" boot profile that runs `systemd-sysinstall`.
- A Hearth on-screen page drives it instead of the stock text screen: it lists the internal drives, warns "Everything on this drive will be erased", and runs sysinstall with the answers filled in.
- The base is Fedora 45.

## Consequences

Easier:

- The user steers the install with the remote, like the rest of Hearth.
- We build on a systemd tool instead of writing our own partitioning and copying code.

Harder:

- The partition layout is hard to undo. The A/B slot sizes the installer writes can't easily be changed on boxes already in homes. Settle the layout before the first public image.
- sysinstall is new and ParticleOS promises no compatibility, so its options can change between systemd versions. Pin the version per release.
- The installer page runs before any profile exists, like the profile picker, and needs its own place in the image.

## To do

Hardware tests, not confirmed:

- What happens at the end: the man page says it reboots, but `--reboot=` defaults to false.
- Running it unattended from a Hearth page.
- Fedora 45's version with our partition definitions.
- research bootc's own install command (`bootc install to-disk`): not compared.

## Turned down

- The stock sysinstall text screen.
- Writing the whole image to the internal disk and letting `systemd-repart` grow it on first boot.

