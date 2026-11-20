# P4 — creating users on a read-only /usr
 
**Sources:** systemd man pages on freedesktop.org (latest = 262, Fedora 45's version [S](https://packages.fedoraproject.org/pkgs/systemd/systemd/fedora-45.html)); useradd/userdel man pages on man.archlinux.org. [C] = primary. [S] = secondary.
 
## Answers
 
1. **useradd/userdel:** Yes, in principle. Every file they list is under `/etc`; homes go in `/home`. Nothing is under `/usr`. [C](https://man.archlinux.org/man/useradd.8.en) The man pages never mention a read-only `/usr`: NOT FOUND, so test it on an image. `userdel` edits the same files; `-r` also removes the home and mail spool, and it refuses while the user has running processes. [C](https://man.archlinux.org/man/userdel.8.en)
2. **systemd-homed:** A service that creates, removes and inspects home areas, optionally encrypted. Records are signed JSON; its keys live in `/var/lib/systemd/home/`. [C](https://www.freedesktop.org/software/systemd/man/latest/systemd-homed.service.html) The record is stored inside the home itself, so homes move between machines (the public key must be copied too). With LUKS2 (Linux disk encryption), even the administrator can't read the data without the user's password. `homectl` creates regular users only; groups you assign must already exist. [C](https://www.freedesktop.org/software/systemd/man/latest/homectl.html)
3. **userdbctl:** Shows users, groups and memberships from every source: classic `/etc/passwd`, `/etc/userdb` drop-ins, homed, dynamic users. It creates nothing, except `load-credentials`, which copies given records into `/etc/userdb/` or `/run/userdb/`. [C](https://www.freedesktop.org/software/systemd/man/latest/userdbctl.html)
## Table
 
| Tool | Creates users? | Where the record lives | On Fedora 45? | Link |
|---|---|---|---|---|
| `useradd` / `userdel` | Yes / removes | `/etc/passwd`, `/etc/shadow`, `/etc/group`, `/etc/gshadow` | [M] Yes, shadow-utils | [C](https://man.archlinux.org/man/useradd.8.en) |
| `systemd-sysusers` (`sysusers.d`) | System users only, not regular ones | `/etc/passwd` and `/etc/group`, written directly; configs in `/usr/lib/sysusers.d` | Part of systemd [M] | [C](https://www.freedesktop.org/software/systemd/man/latest/sysusers.d.html) |
| `homectl` + `systemd-homed` | Regular users only | In the home area; keys in `/var/lib/systemd/home/` | `homectl` is in package `systemd-udev` [C] | [C](https://www.freedesktop.org/software/systemd/man/latest/homectl.html) |
| `userdbctl` | No (inspect only) | Reads all sources, owns none | Yes, in the `systemd` package [C](https://packages.fedoraproject.org/pkgs/systemd/systemd/fedora-45.html) | [C](https://www.freedesktop.org/software/systemd/man/latest/userdbctl.html) |
 
## homed on Fedora 45
 
- Fedora 45's package metadata (your `fedora45-primary.xml.zst`) puts `/usr/bin/homectl` and `/etc/systemd/homed.conf` in `systemd-udev`. [C] It lists only some paths, so the `systemd-homed` service file is NOT CONFIRMED. Recommendation: on a Fedora 45 box, run `rpm -qf /usr/lib/systemd/systemd-homed`.
- Fedora's authselect has a `with-systemd-homed` feature. [C](https://raw.githubusercontent.com/authselect/authselect/master/profiles/local/README)
- A Fedora Magazine article says that since Fedora 41 the service is enabled by default and an SELinux policy ships, but the PAM feature is off. [S](https://fedoramagazine.org/unlocking-the-future-of-user-management-with-systemd-homed/)
- Official "supported" statement for Fedora 45: NOT FOUND.
 
