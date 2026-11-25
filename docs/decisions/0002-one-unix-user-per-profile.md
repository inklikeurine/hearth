# Decision 0002: One Unix user per profile, one active session at a time

Related: [Secret store: oo7](0001-keyring-oo7.md).

## Context

Profiles inside a single Unix user switch fast but only separate data at app level. Choosing the secret store showed two problems with that:

- All profiles would share one keyring.
- Third-party Flatpak apps keep their data in the user's home folder, so every profile would share one login per app. Only Jellyfin and the Firefox web apps would be separated.

The lead decided real separation matters more than fast switching, and accepted slower switching. The design hasn't been researched yet.

## Decision

- Each profile is its own Unix user.
- One user session is active at a time; when another profile logs in, the old session ends.
- Profiles are protected by a PIN, if that turns out to be possible (to be researched; see [Profile PINs](0018-profile-pins-self-managed.md)).
- A profile's keyring is set up when the profile is created.
- The user creates the default profile during installation.

## Consequences

Easier:

- Real separation of files, logins, keyrings and third-party app data between profiles.
- Parental controls and screen time can key on the Unix user (to verify).
- Each profile has its own keyring.

Harder:

- A profile picker has to run outside any profile's session. It's a new component.
- Creating and deleting a profile are privileged jobs (make the user and its home folder, set up the keyring, clean up on delete). 
- Services that exist once per box (remote input, CEC, phone pairing, companion app, ad blocker) have to work whichever profile is active.
- The keyring setup in [Secret store: oo7](0001-keyring-oo7.md) happens at profile creation, not at first boot.
- The Firefox kiosk profiles per app live in each profile's home.

## Not decided

- How a PIN guards a Unix user, and what it can actually protect.

## To do

- The research task "One Unix user per profile" in the lead's log.
- Then a small proof on a Fedora test box: switching, ending the old session, keyring setup.

## Turned down

- Profiles inside one Unix user.
- One user with a separate data folder per profile for each Flatpak app.
- Several sessions active at once for fast switching.
