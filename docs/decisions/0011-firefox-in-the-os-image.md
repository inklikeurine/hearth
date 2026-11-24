# Decision 0011: Firefox ships in the OS image, pinned and tested, not as a Flatpak

Related: design doc, sections 2, 3, 5.2, 5.3 and 12.1; [Firefox outside customising](../research/2026-11-19-firefox-outside-customising.md).

## Context

Firefox is the engine behind out web apps. It ships inside the OS image. 
Firefox is also shaped from outside (policies, kiosk mode, an unsandboxed AutoConfig script) in ways that touch its internals and can break on updates.

## Decision

Firefox ships in the OS image. Hearth pins the version and tests it with a TV-navigation extension, `policies.json`, kiosk mode and AutoConfig before it ships; it updates and rolls back with the OS.

## Consequences

- Every Firefox security release needs a new Hearth image, so automated image builds are required for 1.0.
- One update path: a Firefox update can't break a box on its own, and A/B rollback covers it.
- No Freedesktop runtime to download and keep in memory, which helps the 4 GB budget.
- Codecs for the image's Firefox come through the image's own codec plan (design doc, section 5.3), not Flathub's runtime extensions.

## Turned down

- Mozilla's Flatpak from Flathub.
