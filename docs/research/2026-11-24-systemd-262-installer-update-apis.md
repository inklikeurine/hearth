# systemd 262: the installer gained an API, the update API is being replaced

*Claude, 2026-11-24, after the lead asked whether something had changed in the installer area. Tags: [C] = read in systemd's own release notes ([NEWS at v262](https://github.com/systemd/systemd/blob/v262/NEWS)); [T] = seen in the dev box, Fedora 45 with systemd 262.1.*

| What | Finding |
|---|---|
| Installer | `systemd-sysinstall` gained a Varlink API, `io.systemd.SysInstall`. (Varlink is systemd's way for one program to call another over a socket file.) It runs an installation with progress notifications, passes credentials and locale, keymap and time zone, and lists candidate drives with fit and size [C]. Fedora 45 ships it, with the calls `ListCandidateDevices` and `Run` [T] |
| Who may call it | Its socket is on by default and open to any local user, but both calls ask polkit for an administrator's password [T]. A remote can't type one, so Hearth needs a polkit rule for the installer page's user only |
| Updates, D-Bus | `systemd-sysupdated`'s D-Bus API (a way for programs to call each other) is to be removed in systemd 263 [C] |
| Updates, replacement | Clients are to talk to `systemd-sysupdate` over Varlink. In 262 that API only lists features and components and checks for newer versions [C], so it can't yet install an update |
| Updates, status | `systemd-sysupdate` is marked experimental again, with "more breaking changes" to come, and moved out of the normal program path [C] |
| Update units | `systemd-sysupdate.service` and `.timer` are renamed `systemd-sysupdate-update.service` and `.timer`, with compatibility links [C] |
| Secrets sealed to a TPM | Now pinned to that TPM's own key [C]. An older finding says systemd before 262 can't open them, which matters for rollback across that version; not re-checked today |

## What it means (Claude's reading, not decided)

- Installer: the Hearth page can use the API instead of running the program with pre-filled answers. It gives a drive list and a progress bar. This fits the Installer decision.
- Updates: don't build the Updates page on the D-Bus API. Put one small Hearth program in front of `systemd-sysupdate`, so only that changes when systemd does.
- sysupdate stays the choice, but it's a moving target. Pinning systemd per release, already agreed, matters more.

## Not confirmed

- Nothing was run: no install and no update.
- Whether 263 arrives in Fedora 45.
