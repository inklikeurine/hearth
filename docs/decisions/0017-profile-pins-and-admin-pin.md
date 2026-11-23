# Decision 0017: Profile PINs are opt-in and self-managed; parental controls require an admin PIN, which can reset a kid's PIN

Accepted 2026-11-22 by the lead. Replaces decision 0003 (profile PINs opt-in, managed by each profile, recovered with a hint; accepted 2026-11-18). Related: [One Unix user per profile](0002-one-unix-user-per-profile.md), [Boot shows the profile picker](0004-boot-shows-profile-picker.md), design doc, sections 13 and 13.2; [polkit rules](../research/P5-polkit-admin.md).

## Why it changed

Decision 0003 had no admin: each profile managed its own PIN, and "an admin profile that resets other profiles' PINs" was turned down. Its consequences noted that the design doc's parental-control rows (leaving a kid profile, extending screen time) relied on an admin PIN and no longer fit, and the question went to the lead's log.

The lead's answers, 2026-11-22:
- "Parental controls will require an admin pin. An admin pin setup flow should be tagged for the onboarding/first install flow at a later date."
- Asked whether the admin PIN may also reset a kid's profile PIN, which reverses part of 0003: yes ("Admin can reset PINs too").
- Asked when the admin PIN is required: once parental controls are first used, not on every box ("Once controls are used").

## Decision

Kept from decision 0003:
- Every profile, the default one included, can have a PIN.
- PINs are off by default for every profile.
- A profile sets, changes and removes its own PIN.
- Forgotten PIN: the user can set a hint shown on the PIN screen.

New:
- Parental controls require an admin PIN.
- The admin PIN is asked for when someone first sets up a kid profile or a restriction. A household that never uses parental controls never meets it.
- The admin PIN can reset a kid profile's PIN. This replaces 0003's "there is no admin override" for kid profiles.
- The admin PIN's setup flow is tagged for the onboarding / first-install flow, designed later.

## Consequences

Easier:

- The parental-control rows in section 13 fit again: leaving a kid profile and extending screen time take the admin PIN.
- A child who sets a PIN and forgets it, or locks a parent out of the kid profile, can be reset by the admin.
- Households without children never see an admin PIN.

Harder:

- Hearth needs admin machinery after all. The polkit research found a way: an admin is a Unix group plus Hearth's own polkit rules in the read-only image, checked by Hearth's system services ([polkit rules](../research/P5-polkit-admin.md)). Not built or tested.
- Resetting another user's PIN is a privileged job, so it goes through the narrowly scoped helper that creating and deleting profiles already need ([One Unix user per profile](0002-one-unix-user-per-profile.md)). How a PIN guards a Unix user is still open (design doc, section 13.2), and the reset depends on that answer.
- A forgotten admin PIN has no recovery yet, and it guards the controls, so the setup flow has to cover it.

## Not decided

- Whether the admin PIN belongs to an admin profile (that profile's own PIN, required once parental controls are on) or is a separate box-wide PIN.
- Whether the admin PIN can reset an adult profile's PIN, or only a kid's.
- What else the admin PIN guards beyond section 13's rows: settings, adding apps, creating and deleting profiles.
- Recovering a forgotten admin PIN.
- Whether authenticating with a paired device (the phone, for example) is feasible and safe for any recovery (carried over from 0003).
- Whether a locked profile can be deleted without wiping the box (carried over from 0003).
- How a PIN guards a profile: Hearth checks it itself, or it's set as the Unix password (open, "later", design doc, section 13.2).

## Turned down

- No admin at all, each profile managing only itself (decision 0003): leaves parental controls with nothing to lock them.
- An admin PIN set on every box at first install: the lead chose to ask for it only once parental controls are used.
- PINs on by default: turned down in 0003; still turned down.
