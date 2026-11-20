# Rollback "healthy" signal and BIOS boot: what the docs already answer (2026-11-19)

Two research prompts checked in a design session against systemd's and mkosi's own docs. Tags: [C] read in the project's docs or source today; [M] inference or sketch, not tested.

## Marking a boot good only when the home screen is up

- How it works [C, [Automatic Boot Assessment](https://github.com/systemd/systemd/blob/main/docs/AUTOMATIC_BOOT_ASSESSMENT.md)]: systemd-boot keeps a try counter in each boot entry's file name (for example `+3-1`) and counts down on every attempt; when the tries run out it falls back to the next entry. `systemd-bless-boot.service` removes the counter ("good") only once `boot-complete.target` is reached. `systemd-boot-check-no-failures.service` can also be required, so any failed service blocks the "good" mark.
- Holding back "good" until Hearth's shell works [C, same page]: put the check in a unit ordered `Before=boot-complete.target`, pulled in by the target (`RequiredBy=`), not the other way round. systemd's own walkthrough describes this pattern: a `graphical-session-good.service` that waits for a D-Bus call from the user's session before it exits successfully.
- Needs UEFI [C, `src/boot/boot.c`]: the counter is renamed by systemd-boot, a UEFI program, which reports the renamed path to the OS through an EFI variable (`LoaderBootCountPath`). Other boot loaders can implement the same renaming (the doc invites it); whether GRUB does is not checked.

Sketch [M, untested]:

```ini
# /usr/lib/systemd/system/hearth-shell-healthy.service
[Unit]
Description=Hearth home screen came up
Before=boot-complete.target

[Service]
Type=oneshot
RemainAfterExit=yes
# Waits for the shell to call org.hearth.Health1.Ready over D-Bus, fails after 3 minutes.
ExecStart=/usr/libexec/hearth/wait-shell-ready --timeout=180

[Install]
RequiredBy=boot-complete.target
```

Still to confirm in the image: how systemd-sysupdate's A/B entries carry the counter (the `+N` tries in the entry or UKI name), and what "fails" looks like on a box that hangs instead of erroring (the timeout above).

## BIOS-only boxes

- One image can boot on both UEFI and BIOS [C, [mkosi man page](https://github.com/systemd/mkosi/blob/main/mkosi/resources/man/mkosi.1.md), `BiosBootloader=`]: set both `Bootloader=` (UEFI) and `BiosBootloader=grub`; mkosi adds a 1 MB GRUB BIOS boot partition next to the ESP.
- Automatic rollback on BIOS depends on the boot loader doing the counting (above), so BIOS boxes would lose it unless GRUB implements it [M].
- Still open: whether machines at the hardware floor (Intel 7th gen / AMD Raven Ridge, 2017–2018) all ship UEFI firmware, so "BIOS-only" only means "UEFI switched off in settings". That is the one narrow research prompt left (lead's log).
