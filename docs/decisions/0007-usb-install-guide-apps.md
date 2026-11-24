# Decision 0007: The USB install guide covers four apps; the wiki is written for 1.0.0

Related: design doc, section 2 ([OS image and updates](../design.md)); [Release files](0006-release-files-raw-image-signed-checksum.md).

## Context

Hearth builds no flasher. The guide tells an end user how to put our release file on a USB stick. Research, including the picture-by-picture outline: [installer research](../research/2026-11-19-installer-flasher-and-usb-install.md).

- Fedora Media Writer: Windows, macOS, Linux; writes the image and reads it back to verify.
- Rufus: Windows only; built-in checksum button.
- balenaEtcher: Windows and macOS; Electron.
- GNOME Disks: Linux; "Restore Disk Image".

## Decision

- The guide lives on the GitHub wiki and covers four apps: Fedora Media Writer (recommended, on all three systems), Rufus (Windows), balenaEtcher (Windows and macOS), and GNOME Disks (Linux).
- A different app per platform is fine.
- The wiki is written at 1.0.0, not before.

## To do

Hardware tests, not confirmed:

- Whether each app opens our `.xz` as downloaded (Media Writer's file picker, Rufus's mode choice, Etcher and GNOME Disks `.xz` support).
- Windows SmartScreen and macOS security prompts on current builds.
