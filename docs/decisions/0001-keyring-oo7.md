# Decision 0001: oo7 is Hearth's secret store

Related: design doc, section 4.

## Context

Apps save logins through the freedesktop Secret Service, and Flatpak apps through the Secret portal. Hearth has no desktop and no typed login password, so the store must unlock without one and never show a prompt.

Findings from the [secret store research](../research/2026-11-19-keyring-store.md):
- oo7 can unlock from a systemd credential, has its own Flatpak portal backend, and is the Secret Service provider Fedora 45 moved to. It is still beta (0.7.0 beta in Fedora 45; no stable 0.7.0). Its daemon has no network access.
- With no password typed by a person, the keyring password has to live on the box. The keyring is not a vault.

## Decision

Hearth uses oo7: `oo7-daemon` as the Secret Service and `oo7-portal` as the Flatpak Secret portal backend, both in the OS image.
If the keyring is lost, a new blank one is created and the user is notified that they'll need to re-login to services.

## Consequences

Easier:

- The same store Fedora ships by default; one small Rust daemon (about 9 MB in a forum sample, not measured by us).
- Flatpak apps get their per-app secret through the portal.

Harder:

- oo7 is beta, and upstream has open bugs, mostly about migrating old keyrings, which a fresh image doesn't have.
- Needs a portal config file, and the session has to start `graphical-session.target`.
- Each profile is its own Unix user ([One Unix user per profile](0002-one-unix-user-per-profile.md)), so each has its own keyring.

## Turned down

- gnome-keyring: no prompt-free unlock except an empty password (unencrypted file); being replaced in Fedora.
- KeePassXC: a GUI app that needs someone to open a database; no Flatpak portal backend.
- KDE Wallet: already ruled out.
- No secret store: apps that save logins fail or keep them in plain files.
