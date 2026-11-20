# Decision 0008: Optional hardware is supported but never assumed

Accepted 2026-11-19 by the lead. Related: design doc, sections 2, 6 and 10.

## Context

Hearth targets old laptops, desktops and mini-PCs reused as TV boxes. Many lack add-on hardware a design might take for granted: Wi-Fi in the first-boot wizard, Bluetooth for headphones and pairing, a CEC remote for power-on and testing.

The hardware floor in the design doc (4 GB RAM, x86-64, Intel 7th gen / AMD Raven Ridge-class graphics, NVIDIA supported) is separate and unchanged.

## Decision

These are recommended and supported by default, but never assumed present:

- on-board Wi-Fi
- Bluetooth
- USB infrared receivers
- gamepads
- HDMI-CEC

"Supported by default" means their drivers and features ship switched on with sensible defaults, including default key maps for infrared and Bluetooth remotes.

## Consequences

- Hearth detects which of the five each box has. The first-boot hardware check records it with the hardware tier.
- Every core path works with none of the five present: install, first-boot setup, recovery, settings and updates.
  - Network setup works over Ethernet; Wi-Fi is one option.
  - Setup and recovery can't rely on a Bluetooth or CEC remote.
- Features that need one of the five appear only when it's there, or say what's missing:
  - the Wi-Fi list
  - Bluetooth pairing and Bluetooth headphones
  - CEC power-on and volume
  - gamepad mapping
- Usability testing covers each supported input, and a box with none of the optional ones attached.

## Turned down

- Treating Wi-Fi, Bluetooth, a CEC remote or a gamepad as required: many old mini-PCs and desktops lack one or more, and setup would dead-end.
- Dropping the hardware floor as well: the lead kept the floor.
