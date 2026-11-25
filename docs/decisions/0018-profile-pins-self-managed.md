# Decision 0018: Users manage their own PINs; accounts with parental controls can't; admins can set any profile's PIN

Replaces decision 0017 (profile PINs and the admin PIN). Related: [One Unix user per profile](0002-one-unix-user-per-profile.md), [Boot shows the profile picker](0004-boot-shows-profile-picker.md), [polkit rules](../research/P5-polkit-admin.md).

## Why it changed

Decision 0017 let a profile remove its own PIN and also required a PIN on every account once parental controls were on, and it still listed "who checks PINs" as open after deciding it. The lead settled it on 2026-11-24: "Users can manage their own pins. Accounts with parental controls enabled can not. Admins can set any profiles pin to a new one, in case its forgotten. Forgetting an admin pin means factory reset."

## Decision

New (the lead, 2026-11-24):
- Users manage their own PINs.
- Accounts with parental controls enabled can't manage theirs.
- Admins can set any profile's PIN to a new one, in case it's forgotten.
- A forgotten admin PIN means a factory reset.

Kept from 0017:
- Every profile, the default one included, can have a PIN. PINs are off by default.
- A user can set a hint, shown on the PIN screen.
- Parental controls require an admin PIN, asked for when someone first sets up a child profile or a restriction. A household that never uses parental controls never meets it. Its setup flow is part of the parental-controls design.
- If parental controls are enabled, all accounts require a PIN to enter, except children's profiles, which need the admin PIN to leave.
- The admin PIN is one settable, box-wide PIN for the family's parents, not any profile's own PIN.
- The admin PIN can guard settings, adding apps, and creating and deleting profiles. Each is a switch; all three are on once an admin PIN exists.
- Permissions come from Unix groups, which can also take permissions away.
- Hearth checks every PIN itself; PINs aren't Unix passwords.

## Claude's reading, to confirm

"All accounts require a PIN" and "users can manage their own PINs" both stand. Read together: once parental controls are on, an adult can change their PIN but not remove it, and an adult with none is asked to set one. Otherwise a child at the picker could open that profile.

## Consequences

- Hearth needs admin machinery: a Unix group plus Hearth's own polkit rules, checked by Hearth's system services. Not built or tested.
- An admin can open any adult's profile by setting its PIN.
- Setting another user's PIN is a privileged job and goes through the profiles service.

## Turned down

- No admin at all, each profile managing only itself.
- An admin PIN set on every box at first install.
- PINs on by default.
