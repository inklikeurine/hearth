# D2: Kernel drivers as system extensions

*Research by a Claude research agent, 2026-11-22, run at the lead's request; copied here as returned. Not re-checked line by line.*

**Short answer:** Yes, a sysext can add kernel modules, but it needs a workaround for the module index. A sysext cannot declare a kernel version. It can only bind to an OS version. Flatcar builds its kernel-module sysexts that way, once per OS release. No project was found that builds such a sysext on the box itself.

## 1. Modules on a read-only /usr

- When a sysext is merged, its `/usr` and `/opt` are laid over the host's with overlayfs (a filesystem that stacks one directory tree on another). Anything outside those two trees is ignored. [C] [systemd-sysext(8)](https://www.freedesktop.org/software/systemd/man/latest/systemd-sysext.html)
- So files under `/usr/lib/modules/<kernel>/` do appear on the host. The man page does not mention kernel modules at all. [C] (same page)
- **The catch:** `modules.dep` is the index `modprobe` reads to find modules, and the host's copy doesn't list the sysext's modules. If the sysext ships its own copy, that copy hides the host's.
  - Flatcar deletes those index files from the sysext.
  - It adds a modprobe `install` hook. The hook mounts a private overlay on `/usr/lib/modules/$(uname -r)`, runs `depmod` (the tool that rebuilds the index) there, then loads the module. [C] [sysext_mangle_kmod](https://github.com/flatcar/scripts/blob/6b689271808788231a33a4c5193dd50d901f721c/build_library/sysext_mangle_kmod)
- Flatcar's build script also restarts `systemd-udev-trigger.service` after the merge when the sysext ships udev rules. [C] [build_sysext](https://github.com/flatcar/scripts/blob/6b689271808788231a33a4c5193dd50d901f721c/build_sysext#L292-L297)

## 2. How a sysext declares what it matches

- The image carries `/usr/lib/extension-release.d/extension-release.<NAME>`.
  - `ID=` must match the host's `ID=`, unless it is `_any`.
  - Then `SYSEXT_LEVEL=` must match, or `VERSION_ID=` if no level is set.
  - `ARCHITECTURE=` is checked against `uname`. [C] [systemd-sysext(8)](https://www.freedesktop.org/software/systemd/man/latest/systemd-sysext.html), [os-release(5)](https://www.freedesktop.org/software/systemd/man/latest/os-release.html)
- systemd's current source code also checks `IMAGE_ID=` and `IMAGE_VERSION=` against the host when the extension sets them. The man page does not list this. [C] [extension-util.c](https://github.com/systemd/systemd/blob/71ee298dc33560bac6435e6cf72feaa7706df379/src/shared/extension-util.c#L92-L122)
- **There is no kernel-version field.** [C] (same sources)
- **When it stops matching:** an extension that fails the check is skipped, not merged, and the boot carries on. The only log line is "No suitable extensions found (N ignored due to incompatible image(s))", and only when nothing at all is merged. [C] [sysext.c](https://github.com/systemd/systemd/blob/71ee298dc33560bac6435e6cf72feaa7706df379/src/sysext/sysext.c#L2184-L2187)
- Flatcar's docs say a version-bound sysext "won't be loaded anymore after an OS update." [C] [Flatcar sysext docs](https://www.flatcar.org/docs/latest/sys-ext/)
- The agent's recommendation: put `IMAGE_VERSION=` (or `VERSION_ID=`) in the extension-release file. Hearth ships the kernel inside each A/B image, so binding to the image version binds the extension to the kernel too.

## 3. Projects

- **Flatcar:** official NVIDIA driver and ZFS sysexts, "built with every Flatcar release" in Flatcar's own CI. The box downloads them at provisioning. They are pinned with `VERSION_ID=` set to the exact Flatcar version. [C] [Using NVIDIA](https://www.flatcar.org/docs/latest/os-config/host-config/using-nvidia/), [build_sysext L299-301](https://github.com/flatcar/scripts/blob/6b689271808788231a33a4c5193dd50d901f721c/build_sysext#L299-L301)
- **sysext-bakery:** its NVIDIA sysext "does not include the kernel module." [C] [nvidia_runtime](https://flatcar.github.io/sysext-bakery/nvidia_runtime)
- **fedora-sysexts/community:** its NVIDIA sysext is userspace only. Its README says the kernel modules must already be on the host. [C] [repo](https://github.com/fedora-sysexts/community)
- **A project that builds a kernel-module sysext on the box, per kernel:** NOT FOUND.

## Not confirmed

- Whether `systemd-modules-load` or udev coldplug at boot runs before `systemd-sysext.service`. If it does, modules for hardware found at boot won't load without a re-trigger.
- Which systemd release added the `IMAGE_VERSION` matching, and whether Fedora 45's systemd has it.
- Whether mkosi writes `IMAGE_VERSION` into os-release [M]. Hearth would rely on that.
- After a rollback to the other A/B slot, the old slot's sysext still has to be on disk. Needs testing.
- Modules needed in the initrd (the small early-boot system), such as nvidia-drm for early display, are not covered by this approach.
