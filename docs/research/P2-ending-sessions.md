# P2 — ending sessions, user managers, and linger
 
**Sources:** systemd man pages on freedesktop.org (logind.conf checked in versions 258, 260 and 262). [C] = a man page says it. [M] = from memory.
 
## 1 and 2. What each one ends
 
| Command or setting | What it ends | Default | Link |
|---|---|---|---|
| `loginctl terminate-session` | One session: kills all its processes, frees its resources. | Command, none | [C](https://www.freedesktop.org/software/systemd/man/latest/loginctl.html) |
| `loginctl terminate-user` | All sessions of one user: kills all their processes, frees the user's runtime resources. | Command, none | [C](https://www.freedesktop.org/software/systemd/man/latest/loginctl.html) |
| `KillUserProcesses=` | At logout, kills the session's scope (the unit holding its processes) and everything in it. If false, the scope is left running ("abandoned"). | Man pages: `yes`. Fedora: NOT FOUND | [C](https://www.freedesktop.org/software/systemd/man/latest/logind.conf.html) |
 
- `KillOnlyUsers=` and `KillExcludeUsers=` override it; `root` is excluded by default. [C](https://www.freedesktop.org/software/systemd/man/latest/logind.conf.html)
- The man page says defaults are set at compile time, so a distribution can differ. Fedora's value: NOT FOUND in man pages. [M] I believe Fedora ships `no`; unverified. Recommendation: on a Fedora 45 image, run `systemd-analyze cat-config systemd/logind.conf`.
- Not stated: whether `terminate-user` also stops `user@.service`.
## 3. When the last session ends
 
- logind starts a user manager (a per-user copy of systemd, `user@UID.service`) for each logged-in user. [C](https://www.freedesktop.org/software/systemd/man/latest/systemd-logind.service.html)
- After the last session ends, `user@.service` stays for `UserStopDelaySec=` (default 10 s). `0` stops it at once; `infinity` never stops it after first login. [C](https://www.freedesktop.org/software/systemd/man/latest/logind.conf.html)
- Processes the user manager starts belong to `user@.service`. Processes started elsewhere (sshd, a display manager) sit in session scopes. [C](https://www.freedesktop.org/software/systemd/man/latest/user@.service.html)
- So `KillUserProcesses=` targets scopes. Processes under `user@.service` may keep running depending on linger settings. [C](https://www.freedesktop.org/software/systemd/man/latest/logind.conf.html)
- What happens to `user@.service`'s own processes when it stops: NOT FOUND in these pages.
- Whether a Flatpak app or Firefox lands in a session scope or in `user@.service`: NOT FOUND; the pages only say it depends on who started it. Recommendation: launch one and run `systemd-cgls`.
## What linger changes
 
`loginctl enable-linger` starts the user manager at boot and keeps it after logouts, so users who aren't logged in can run long-running services. [C](https://www.freedesktop.org/software/systemd/man/latest/loginctl.html)
 
