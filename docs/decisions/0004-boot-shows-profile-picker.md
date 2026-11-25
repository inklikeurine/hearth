# Decision 0004: Boot shows the profile picker, except for one unlocked profile; each keyring unlocks from a sealed random password

Related: [Secret store: oo7](0001-keyring-oo7.md), [One Unix user per profile](0002-one-unix-user-per-profile.md), [Profile PINs](0018-profile-pins-self-managed.md).

## Context

Two rules for boot: with several profiles, boot shows the profile picker; with one profile that is locked, boot also shows it. [Profile PINs](0018-profile-pins-self-managed.md).

For the keyring, the [secret store research](../research/2026-11-19-keyring-store.md) found that `systemd-creds` uses TPM2 when the box has one and also uses a host key stored on the box. oo7's daemon can read the password from a systemd credential. An empty password would store the keyring unencrypted.

## Decision

- At boot, if there is more than one profile, the box shows the profile picker.
- At boot, if there is one profile and it is locked (its PIN is on), the box shows the profile picker.
- The box logs in straight away only when there is exactly one profile and it has no PIN.
- A locked profile's PIN is entered on the picker, after the profile is chosen.
- Each profile's keyring is unlocked by a random password made when the profile is created. The password is sealed for that Unix user with `systemd-creds` and handed to `oo7-daemon` as a systemd credential.

## Consequences

Easier:

- A PIN does something at boot: a locked profile can't be reached without it.
- No typed password and no prompt anywhere in normal use of an unlocked single-profile box.

Harder:

- The picker runs before any profile's session exists, so it can't be part of a profile. It needs its own place in the image (also noted in [One Unix user per profile](0002-one-unix-user-per-profile.md)).
- A cleared TPM makes every profile's sealed password unreadable and loses every keyring. A reset path is needed.
- `/var/lib/systemd` and each profile's home must survive A/B updates. Keyring files and credentials stay out of settings and support bundles.

## Turned down

- The default profile always logs in at boot.
- Empty keyring password.
- A typed password at boot.
