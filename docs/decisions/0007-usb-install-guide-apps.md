# Decision 0007: The USB install guide covers four apps; the wiki is written for 1.0.0

Accepted 2026-11-18 by the lead. Related: design doc, section 2 ([OS image and updates](../design.md)); [Release files](0006-release-files-raw-image-signed-checksum.md).

## Context

Hearth builds no flasher. The guide tells an end user how to put our release file on a USB stick. Research, including the picture-by-picture outline: [installer research](../research/2026-11-19-installer-flasher-and-usb-install.md).

- Fedora Media Writer: Windows, macOS, Linux; Qt, GPL-2.0, not Electron; writes the image and reads it back to verify; has a "Custom image" button.
- Rufus: Windows only; GPL-3.0+; built-in checksum button (the only app here with one). Checks for updates by default if the file is named `rufus.exe`.
- balenaEtcher: Windows and macOS; Electron; analytics removed in 2.1.2.
- GNOME Disks: Linux; "Restore Disk Image".

## Decision

- The guide lives on the GitHub wiki and covers four apps: Fedora Media Writer (recommended, on all three systems), Rufus (Windows), balenaEtcher (Windows and macOS), and GNOME Disks (Linux).
- A different app per platform is fine.
- Android is dropped.
- The wiki is written at 1.0.0, not before.

## Consequences

Easier:

- One recommended app per system, with a short list of alternatives.

Harder:

- balenaEtcher is Electron. That's allowed for users (only our own development avoids Electron), and the wiki says so.
- Rufus's update check and Media Writer's release-list fetch run on the user's PC, not the box. The wiki warns about the Rufus one.
- Every picture has to be captured on a real machine.

## To do

Hardware tests, not confirmed:

- Whether each app opens our `.xz` as downloaded (Media Writer's file picker, Rufus's mode choice, Etcher and GNOME Disks `.xz` support).
- The same image written on Windows, macOS and Linux, booting on the OptiPlex.
- Windows SmartScreen and macOS security prompts on current builds.

## Not decided

- Raspberry Pi Imager is left out only because the lead's list didn't include it. That's Claude's reading, not confirmed by the lead.

## Turned down

- Raspberry Pi Imager: starts with a Pi device list; statistics on by default.
- Ventoy: no macOS.
- EtchDroid (Android): dropped by the lead.
