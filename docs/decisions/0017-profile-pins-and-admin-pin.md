# Decision 0017: Profile PINs are opt-in and self-managed; parental controls require an admin PIN, which can reset a kid's PIN

Related: [One Unix user per profile](0002-one-unix-user-per-profile.md), [Boot shows the profile picker](0004-boot-shows-profile-picker.md), design doc, sections 13 and 13.2; [polkit rules](../research/P5-polkit-admin.md).

- Parental controls will require an admin pin. An admin pin setup flow should be tagged for the onboarding/first install flow at a later date.
- The admin PIN may also reset a kid's profile PIN.
- When the admin PIN is required: once parental controls are first used/setup.

## Decision

- Every profile, the default one included, can have a PIN.
- PINs are off by default for every profile.
- A profile sets, changes and removes its own PIN.
- Forgotten PIN: the user can set a hint shown on the PIN screen.
- Parental controls require an admin PIN.
- The admin PIN's setup flow is tagged for the Parental Controls design.
- If parental controls are enabled, all accounts require a pin to enter, except childrens profiles, which the admin pin to leave.
- The admin PIN is asked for when someone first sets up a child profile or a restriction. A household that never uses parental controls never meets it.
- The admin PIN can reset any profile's PIN.
- The admin PIN *can* guard settings, adding apps, and creating and deleting profiles: each is a switch the admin sets, with sensible defaults.
  - Defaults: once an admin PIN is required/enabled, all three are guarded; the admin can switch any of them off.
- The admin PIN is one settable, box-wide PIN for the family's parents, not any profile's own PIN .
- Permissions come from Unix groups: a profile outside a group that grants a permission doesn't have it, and groups can also be used to take permissions away.
- Hearth checks every PIN itself; PINs aren't set as Unix passwords.
- A forgotten admin PIN is recovered only by a factory reset.

## Consequences

Easier:

- The parental-control rows in section 13 fit again: leaving a kid profile and extending screen time take the admin PIN.
- A child who sets a PIN and forgets it, or locks a parent out of the kid profile, can be reset by the admin.
- Households without children never see an admin PIN.

Harder:

- Hearth needs admin machinery. The polkit research found a way: an admin is a Unix group plus Hearth's own polkit rules in the read-only image, checked by Hearth's system services ([polkit rules](../research/P5-polkit-admin.md)). Not built or tested.
- Resetting another user's PIN is a privileged job, so it goes through the narrowly scoped helper that creating and deleting profiles already need ([One Unix user per profile](0002-one-unix-user-per-profile.md)). How a PIN guards a Unix user is still open (design doc, section 13.2), and the reset depends on that answer.

## Not decided

- How a PIN guards a profile: Hearth checks it itself, or it's set as the Unix password (open, "later", design doc, section 13.2).

## Turned down

- No admin at all, each profile managing only itself.
- An admin PIN set on every box at first install.
- PINs on by default.
