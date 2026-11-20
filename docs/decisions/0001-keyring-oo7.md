# Decision 0001: oo7 is Hearth's secret store

Accepted 2026-11-18 by the lead. Related: design doc, section 4.

## Context

Apps save logins through the freedesktop Secret Service, and Flatpak apps through the Secret portal. Hearth has no desktop and no typed login password, so the store must unlock without one and never show a prompt. KWallet is already ruled out (design doc, section 2).

Findings from the [secret store research](../research/2026-11-19-keyring-store.md):

- oo7 can unlock from a systemd credential, has its own Flatpak portal backend, and is the Secret Service provider Fedora 45 moved to. It is still beta (0.7.0 beta in Fedora 45; no stable 0.7.0). Its daemon has no network access.
- gnome-keyring can only unlock without a password if the keyring password is empty, which stores the keyring unencrypted. Fedora is replacing it.
- KeePassXC needs a database opened in its GUI and has no Flatpak portal backend.
- With no password typed by a person, the keyring password has to live on the box. The keyring is not a vault.

## Decision

Hearth uses oo7: `oo7-daemon` as the Secret Service and `oo7-portal` as the Flatpak Secret portal backend, both in the OS image. gnome-keyring, KDE Wallet and KeePassXC are not used.

## Consequences

Easier:

- The same store Fedora ships by default; one small Rust daemon (about 9 MB in a forum sample, not measured by us).
- Flatpak apps get their per-app secret through the portal.

Harder:

- oo7 is beta, and upstream has open bugs, mostly about migrating old keyrings, which a fresh image doesn't have.
- Needs Fedora 45 or newer; the daemon isn't packaged for Fedora 44.
- Needs a portal config file, and the session has to start `graphical-session.target`.
- Moving to another store later means every Flatpak app loses the key to its saved logins, and users sign in again.
- Each profile is its own Unix user ([One Unix user per profile](0002-one-unix-user-per-profile.md)), so each has its own keyring.

## Not decided (Claude's proposal)

- Recovery when the keyring or its password is lost (a cleared TPM, for example), and a "Saved logins" settings page to forget or reset logins.

## To do

- Decide the item above.
- Test on real hardware: unlock on first boot and after reboot, a Flatpak login surviving a reboot, a locked keyring with no desktop, a cleared TPM, a box with no TPM.
- Measure RAM.
- Check oo7's AI policy before contributing anything upstream (none found in its repo).

## Turned down

- gnome-keyring: no prompt-free unlock except an empty password (unencrypted file); being replaced in Fedora.
- KeePassXC: a GUI app that needs someone to open a database; no Flatpak portal backend.
- KDE Wallet: already ruled out.
- No secret store: apps that save logins fail or keep them in plain files.
