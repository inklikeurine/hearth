# P1 — greetd sessions and the logind seat
 
**Sources:** a local copy of greetd 0.10.3 (a snapshot, no git history), read directly. Links point to the same files on git.sr.ht; I couldn't open them, and line numbers are from the local copy. logind pages are from freedesktop.org.
 
## 1. Starting a session
 
- The greeter (the login-picker program) sends `create_session` with the user name, answers password prompts, then sends `start_session` with the command to run. The session starts after the greeter exits. [C](https://git.sr.ht/~kennylevinsen/greetd/tree/master/item/man/greetd-ipc-7.scd)
- A separate worker process, running as root, does the work: a PAM (Linux's login-check system) password check, a new process session (`setsid`), terminal setup, then opening the PAM session. [C](https://git.sr.ht/~kennylevinsen/greetd/tree/master/item/greetd/src/session/worker.rs) lines 133–232
- It then forks. The child drops to the user (`initgroups`, `setgid`, `setuid`) and runs `/bin/sh -c "exec <command>"` with PAM's environment. The root parent waits, then closes the PAM session. [C](https://git.sr.ht/~kennylevinsen/greetd/tree/master/item/greetd/src/session/worker.rs) lines 256–322
- The greeter runs as `user` in `[default_session]`, default `greeter`. It starts without a password check, in session class "greeter". [C](https://git.sr.ht/~kennylevinsen/greetd/tree/master/item/greetd/src/context.rs) lines 85–127
## 2. `default_session` and `initial_session`
 
- `default_session` is the greeter. It starts whenever no session is running (for example after logout), or at launch when no initial session exists. [C](https://git.sr.ht/~kennylevinsen/greetd/tree/master/item/man/greetd-1.scd)
- `initial_session` is optional auto-login: no password check, runs once per boot. A runfile (marker file, default `/run/greetd.run`) stops repeats. [C](https://git.sr.ht/~kennylevinsen/greetd/tree/master/item/greetd/src/server.rs) lines 242–265
```toml
[default_session]
command = "agreety -c sway"
[initial_session]
command = "sway"
user = "john"
```
From the greetd(5) examples. [C](https://git.sr.ht/~kennylevinsen/greetd/tree/master/item/man/greetd-5.scd)
 
## 3. How the new session gets the seat
 
- A seat is all hardware for one workplace: graphics, usually keyboard and mouse, possibly sound. Unassigned devices go to `seat0`. [C](https://www.freedesktop.org/software/systemd/man/latest/sd-login.html)
- greetd never calls logind itself. It hard-codes `XDG_SEAT=seat0`, sets `XDG_VTNR` (virtual-terminal number), then opens the PAM session; `pam_systemd` registers it with logind. [C](https://git.sr.ht/~kennylevinsen/greetd/tree/master/item/greetd/src/session/worker.rs) lines 182–232; [C](https://www.freedesktop.org/software/systemd/man/latest/pam_systemd.html)
- Graphics and input: the session's controller (typically the compositor [M]) calls logind over D-Bus. `TakeControl()` claims the session; `TakeDevice()` returns an open file handle per device. Only works while the session is active. [C](https://www.freedesktop.org/software/systemd/man/latest/org.freedesktop.login1.html)
- Sound and similar: devices tagged `uaccess` in udev get file permissions (ACLs) for the active session's user, updated as sessions switch. [C](https://www.freedesktop.org/software/systemd/man/latest/sd-login.html)
- greetd ships no PAM files. Whether Fedora's greetd PAM files load `pam_systemd`: NOT FOUND. Recommendation: check in a test image.
