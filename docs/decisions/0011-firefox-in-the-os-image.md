# Decision 0011: Firefox ships in the OS image, pinned and tested, not as a Flatpak

Accepted 2026-11-19 by the lead. Related: design doc, sections 2, 3, 5.2, 5.3 and 12.1; [Firefox outside customising](../research/2026-11-19-firefox-outside-customising.md).

## Context

Firefox is the engine behind four of the six 1.0 apps. It could ship inside the OS image or as Mozilla's Flatpak from Flathub. The design doc says Hearth's own parts are never Flatpaks and left "bundled apps: image or Flatpak?" open. In the network-allowlist work the lead said Hearth supports the latest Firefox release only and pins the version in the image.

Firefox is also shaped from outside (policies, kiosk mode, an unsandboxed AutoConfig script) in ways that touch its internals and can break on updates.

## Decision

Firefox ships in the OS image. Hearth pins the version and tests it with the TV-navigation extension, `policies.json`, kiosk mode and AutoConfig before it ships; it updates and rolls back with the OS. Latest Firefox release only. Whether the package is Fedora's own or a Hearth build is still open.

## Consequences

- Every Firefox security release needs a new Hearth image, so automated image builds are required for 1.0.
- One update path: a Firefox update can't break a box on its own, and A/B rollback covers it.
- No Freedesktop runtime to download and keep in memory, which helps the 4 GB budget.
- Codecs for the image's Firefox come through the image's own codec plan (design doc, section 5.3), not Flathub's runtime extensions.
- Moonlight and the Jellyfin client stay an open question (image or Flatpak).

## Turned down

- Mozilla's Flatpak from Flathub: security fixes reach boxes the day Mozilla ships them, with codecs from the runtime and a sandbox. Turned down because an untested version can reach boxes and break the extension or kiosk setup; holding updates back removes the speed advantage; NVIDIA hardware decode is reported broken in the Flatpak; and it adds the runtime's memory cost and a Flathub connection.
