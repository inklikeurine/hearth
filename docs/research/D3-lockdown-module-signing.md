# D3: Kernel lockdown and module signing

*Research by a Claude research agent, 2026-11-22, run at the lead's request; copied here as returned. Not re-checked line by line.*

**Short answer:** Lockdown "integrity" mode blocks unsigned modules whether Secure Boot is on or off. With Secure Boot off, a key made on the box is not trusted, with one exception: a key the box owner enrolls in the firmware's own key list (UEFI db). Fedora 45's kernel locks down only when Secure Boot is on, and does not force signatures.

## 1. Unsigned modules in integrity mode

- The kernel accepts the module only if it has a valid signature. Otherwise it asks lockdown, and lockdown refuses at integrity level or higher. [C] [signing.c](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/plain/kernel/module/signing.c), [security.h](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/plain/include/linux/security.h)
- Lockdown can be switched on without Secure Boot, with `lockdown=integrity` on the kernel command line or by writing to `/sys/kernel/security/lockdown`. The level can only go up while the box is running. [C] [kernel-parameters](https://www.kernel.org/doc/Documentation/admin-guide/kernel-parameters.txt), [lockdown Kconfig](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/plain/security/lockdown/Kconfig), [lockdown.c](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/plain/security/lockdown/lockdown.c)
- The man page says "Only validly signed modules may be loaded", unless IMA (the kernel's file-integrity checker) vouches for the file. [C] [kernel_lockdown(7)](https://man7.org/linux/man-pages/man7/kernel_lockdown.7.html)

## 2. Which keys the kernel trusts

- Upstream checks modules against the secondary keyring. That keyring holds the keys built into the kernel, plus the .machine keyring, which holds MOK (machine owner keys: keys the owner enrolls through shim, the Secure Boot bootloader). [C] [signing.c](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/plain/kernel/module/signing.c), [system_keyring.c](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/plain/certs/system_keyring.c)
- Fedora patches the kernel to fall back to the .platform keyring, which holds the UEFI db keys and any MOK keys not put in .machine. [C] [Fedora patch-7.3-redhat.patch](https://src.fedoraproject.org/rpms/kernel/blob/f45/f/patch-7.3-redhat.patch)
- With Secure Boot off, MOK keys are skipped entirely; the source code comment reads "MOK/MOKx can not be trusted when secure boot is disabled". UEFI db keys are still loaded. [C] [load_uefi.c](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/plain/security/integrity/platform_certs/load_uefi.c)
- A new key can be added to the secondary keyring at runtime only if a key already trusted has signed it, which a key made on the box cannot be. [C] [system_keyring.c](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/plain/certs/system_keyring.c)
- So, with Secure Boot off, an akmods key is trusted only if it is enrolled in the firmware db. Otherwise it is not trusted.

## 3. The two enforcement switches, and Fedora 45's settings

- `CONFIG_MODULE_SIG_FORCE` is a build-time setting that sets the default of `module.sig_enforce`. With either one on, unsigned modules and modules signed with an unknown key are refused. With both off, they load and mark the kernel as tainted (flagged as running outside code). [C] [module-signing](https://docs.kernel.org/admin-guide/module-signing.html)
- `sig_enforce` can be switched on while the box runs, but not off. [C] [signing.c](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/plain/kernel/module/signing.c)
- Fedora 45 x86_64 sets `MODULE_SIG=y`, leaves `MODULE_SIG_FORCE` unset, and sets `LOCK_DOWN_KERNEL_FORCE_NONE=y` and `LOCK_DOWN_IN_EFI_SECURE_BOOT=y`. That last one is a Fedora patch that turns on integrity lockdown only when Secure Boot is on. [C] [kernel-x86_64-fedora.config (f45)](https://src.fedoraproject.org/rpms/kernel/blob/f45/f/kernel-x86_64-fedora.config), [patch](https://src.fedoraproject.org/rpms/kernel/blob/f45/f/patch-7.3-redhat.patch)
- So with Secure Boot off, stock Fedora has no lockdown and loads unsigned modules. [C], from the code and config above.

**Recommendation (the research agent's, not decided):** "On except boxes that need an outside driver" is doable without Secure Boot. Put `lockdown=integrity` on the command line only on boxes with no outside driver. On boxes that need one, the system could load the driver first and then write `integrity` to the lockdown file.

## Not confirmed

- Whether NVIDIA's driver loads more modules later (for example nvidia-uvm). Any module loaded after lockdown is switched on would be refused unless signed. Needs testing on real hardware.
- Whether cheap boxes' firmware lets the owner enroll a db key while Secure Boot is off. Needs testing.
- Fedora 45 sets `INTEGRITY_CA_MACHINE_KEYRING_MAX=y`, and akmods' key is `CA:FALSE` ([cacert.config.in](https://src.fedoraproject.org/rpms/akmods/blob/rawhide/f/cacert.config.in)). So with Secure Boot on, the akmods MOK key probably lands in .platform and works through Fedora's fallback. Not tested.
- The man7 copy of kernel_lockdown(7) did not show the integrity and confidentiality modes. NOT FOUND there; the modes are confirmed from kernel-parameters only.
