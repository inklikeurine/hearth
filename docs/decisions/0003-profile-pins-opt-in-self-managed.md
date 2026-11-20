# Decision 0003: Profile PINs are opt-in, managed by each profile, recovered with a hint

Accepted 2026-11-18 by the lead. Related: [One Unix user per profile](0002-one-unix-user-per-profile.md).

## Context

Three PIN questions were open: what boot does when the default profile has a PIN, who manages PINs, and how a forgotten PIN is recovered. This record answers the second and third; boot is in [Boot shows the profile picker](0004-boot-shows-profile-picker.md). The lead isn't sure how to build an "admin profile" on Fedora 45.

## Decision

- Every profile, the default one included, can have a PIN.
- PINs are off by default for every profile.
- A profile sets, changes and removes its own PIN. There is no admin override.
- Forgotten PIN: the user can set a hint shown on the PIN screen. There is no other recovery. Authenticating with a previously paired device is a maybe, not decided.
- Still "if possible": research decides how a PIN guards a Unix user.

## Consequences

Easier:

- Nobody meets a PIN unless they turn one on.
- PINs need no admin machinery.

Harder:

- A parent can't reset a child's PIN, and a child can lock their own profile.
- A profile is only protected if its owner turns the PIN on. An adult profile with no PIN can be opened by anyone from the profile picker.
- A forgotten PIN with no hint and no paired-device recovery leaves that profile locked. The other profiles and the box keep working.
- The design doc's parental-control rows that rely on an admin PIN (section 13: leaving a kid profile, extending screen time) no longer fit. Logged as an open question.

## Not decided

- Whether authenticating with a paired device (the phone through KDE Connect, for example) is feasible and safe for recovery.
- Whether an admin profile exists at all, and how to build one on Fedora 45. Parental controls and creating or deleting profiles may need one.
- Whether a locked profile can be deleted without wiping the box.

## To do

- The research task "One Unix user per profile" and the parental-controls question in the lead's log.

## Turned down

- An admin profile that resets other profiles' PINs: the lead chose that each profile manages its own.
- PINs on by default: the lead chose off.
