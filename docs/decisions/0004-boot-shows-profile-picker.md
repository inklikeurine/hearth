# Decision 0004: Boot shows the profile picker, except for one unlocked profile; each keyring unlocks from a sealed random password

Accepted 2026-11-18 by the lead. Related: [Secret store: oo7](0001-keyring-oo7.md), [One Unix user per profile](0002-one-unix-user-per-profile.md), [Profile PINs](0017-profile-pins-and-admin-pin.md).

## Context

The lead set two rules for boot: with several profiles, boot shows the profile picker; with one profile that is locked, boot also shows it. That answers what boot does when the default profile has a PIN, which [Profile PINs](0017-profile-pins-and-admin-pin.md) left open.

For the keyring, the [secret store research](../research/2026-11-19-keyring-store.md) found that `systemd-creds` uses TPM2 when the box has one and also uses a host key stored on the box. oo7's daemon can read the password from a systemd credential. An empty password would store the keyring unencrypted.

## Decision

- At boot, if there is more than one profile, the box shows the profile picker.
- At boot, if there is one profile and it is locked (its PIN is on), the box shows the profile picker.
- The box logs in straight away only when there is exactly one profile and it has no PIN. The lead hasn't yet confirmed this one-profile case in their own words.
- A locked profile's PIN is entered on the picker, after the profile is chosen.
- Each profile's keyring is unlocked by a random password made when the profile is created. The password is sealed for that Unix user with `systemd-creds` (TPM2 if present, plus the host key) and handed to `oo7-daemon` as a systemd credential. No empty password.

## Consequences

Easier:

- A PIN does something at boot: a locked profile can't be reached without it.
- No typed password and no prompt anywhere in normal use of an unlocked single-profile box.

Harder:

- The picker runs before any profile's session exists, so it can't be part of a profile. It needs its own place in the image (also noted in [One Unix user per profile](0002-one-unix-user-per-profile.md)).
- Every normal boot on a multi-profile box needs someone with a remote to pick a profile; the box doesn't start into a profile on its own.
- Not a vault. The sealed password stops someone who copies only the keyring file. A stolen disk is stopped only if the box has a TPM2, and nothing stops someone running as that user.
- A PIN doesn't protect a keyring. The PIN decides who may start a profile's session; once the session starts, its keyring unlocks by itself.
- A cleared TPM makes every profile's sealed password unreadable and loses every keyring. A reset path is needed.
- systemd 262 and later pin TPM2 credentials to the TPM, and older systemd can't open them. Rolling back to an older base release would break them.
- The helper that creates a profile also seals its credential.
- `/var/lib/systemd` and each profile's home must survive A/B updates. Keyring files and credentials stay out of settings and support bundles.

## To do

- The reset path and the "Saved logins" settings page (in the lead's log).
- How the picker takes a PIN while no session exists (research task).
- Hardware tests: unlock on first boot and after reboot, a Flatpak login surviving a reboot, a locked keyring with no desktop, a cleared TPM, a box with no TPM.
- Measure RAM.

## Turned down

- The default profile always logs in at boot: the lead chose a picker for several profiles or a locked one.
- Empty keyring password: stores the keyring unencrypted.
- A typed password at boot: the box has none.
- Whole-disk encryption instead: LUKS with TPM auto-unlock is a separate option that complements this; it doesn't replace it.
