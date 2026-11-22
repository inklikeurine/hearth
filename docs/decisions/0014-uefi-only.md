# Decision 0014: Hearth boots on UEFI only

Accepted 2026-11-22 by the lead ("BIOS-only boxes: sure", answering Claude's proposal). Related: design doc, sections 5.2 and 12.1; [USB install guide](0007-usb-install-guide-apps.md).

## Context

Automatic rollback relies on systemd-boot counting boot attempts, and systemd-boot runs only on UEFI firmware. mkosi can build one image that also boots on old-style BIOS, but rollback there would depend on GRUB doing the counting ([boot assessment and BIOS](../research/2026-11-19-boot-assessment-and-bios.md)). The open question was whether "BIOS-only" boxes exist at the hardware floor at all.

The research ([UEFI at the hardware floor](../research/U1-uefi-at-the-floor.md)) found:

- Windows 8 (2012) required every new PC to ship in native UEFI mode. [C]
- Intel planned to remove the old-BIOS layer (CSM, the Compatibility Support Module) from its platforms by 2020, after Hearth's floor (Kaby Lake, 2017; Raven Ridge, 2018). [C] So floor-era machines have UEFI, with CSM usually still present.
- On those machines CSM is a firmware setting the user can switch off: Dell documents it for 7th-gen-and-newer systems, AMD for Ryzen with Radeon Vega. [C]
- Forum reports say some AMD AM4 boards can't switch CSM off when the graphics card has no UEFI display support. [S]

## Decision

- Hearth's image boots on UEFI only. Claude proposed this; the lead agreed.
- A box that boots in legacy (CSM) mode has to be switched to UEFI first. The USB install guide says so before the first step, with the menu names for common makers.

## Consequences

- One boot path to build and test: systemd-boot with boot counting, so rollback works the same on every box.
- A USB stick written with Hearth simply won't start on a box in legacy mode. No Hearth screen can explain it, so the guide and the troubleshooting page carry the explanation.
- Boxes that can't leave legacy mode (the AM4 reports above) are unsupported.
- Supporting BIOS later stays possible: mkosi can build a hybrid image, with GRUB doing the boot counting.

## Turned down

- Booting on BIOS too, through a hybrid image with GRUB: two boot paths to test, and rollback that works differently on those boxes, for machines the floor mostly excludes anyway.
