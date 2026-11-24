# Decision 0008: Optional hardware is supported but never assumed

Related: design doc, sections 2, 6 and 10.

## Context

Hearth targets old laptops, desktops and mini-PCs reused as TV boxes. Many lack add-on hardware a design might take for granted: Wi-Fi in the first-boot wizard, Bluetooth for headphones and pairing, a CEC remote for power-on and testing.

## Decision

These are recommended and supported by default, but never assumed present:

- on-board Wi-Fi
- Bluetooth
- USB infrared receivers
- gamepads
- HDMI-CEC

"Supported by default" means their drivers and features ship switched on with sensible defaults, including default key maps for infrared and Bluetooth remotes.

## Consequences

- Hearth detects which of the five each box has on boot. The first-boot hardware check records it with the hardware tier. Subsequent boots check if the configuration has changed.
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

- Treating Wi-Fi, Bluetooth, a CEC remote or a gamepad as required.
- Dropping the hardware floor.
