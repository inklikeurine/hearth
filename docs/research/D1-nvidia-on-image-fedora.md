# D1 – NVIDIA and Wi-Fi drivers on a Fedora image (Universal Blue, RPM Fusion)

Read 2026-11-20. Tags: [C] primary source read, [S] secondary, [M] memory.

**Answer.** Universal Blue builds NVIDIA kernel modules ahead of time in a separate image, then copies them into the OS image. RPM Fusion ships "akmod" packages that compile on the machine; akmods rebuilds missing modules at boot after a kernel update.

## 1. Universal Blue / Bazzite

- Separate `akmods-nvidia` and `akmods-nvidia-open` images, built daily per kernel flavour; each caches the matching kernel RPMs. [C] https://github.com/ublue-os/akmods
- Driver source is negativo17 (fedora-nvidia). [C] same page
- Bazzite's build pulls `ghcr.io/ublue-os/akmods-${NVIDIA_FLAVOR}:…` (default `nvidia-open`) as a stage and mounts its `/rpms`. [C] https://raw.githubusercontent.com/ublue-os/bazzite/main/Containerfile (what `build.sh` then does: NOT READ)
- Module signing: CI script `sign-modules.sh` signs each module with `openssl cms` plus the kernel's `sign-file`, using key files listed in `active-signing-keys`. [C] https://raw.githubusercontent.com/ublue-os/akmods/main/build_files/post/sign-modules.sh
- Keys: production keys are named `KSIGN_<date>`, supplied through environment variables, never committed; the default mode is "test". [C] https://raw.githubusercontent.com/ublue-os/akmods/main/Justfile
- Public certificate: `certs/public_key.der` [C] https://api.github.com/repos/ublue-os/akmods/contents/certs; the `ublue-os-akmods-addons` package "installs … our kmods signing key". [C] README
- Users enrol "Universal Blue's key" as a MOK (a Secure Boot key you add yourself); password `universalblue`. [C] https://docs.bazzite.gg/General/Installation_Guide/secure_boot/

So the signer is Universal Blue's own CI, with a Universal Blue-generated key.

## 2. RPM Fusion

**NVIDIA** [C] https://rpmfusion.org/Howto/NVIDIA
- Install `akmod-nvidia` plus `xorg-x11-drv-nvidia`. `kmod-nvidia` is offered for RHEL/CentOS.
- The page also says `kmod-nvidia` in nonfree comes "pre-built" in proprietary and open variants. Whether Fedora gets per-kernel prebuilt RPMs: NOT CONFIRMED.
- Secure Boot: the module must be signable before install, or the screen goes blank.

**Broadcom wl**
- `broadcom-wl` (licence, README, config files; licence "Redistributable, no modification permitted") and `akmod-wl`, both in nonfree. [C] https://download1.rpmfusion.org/nonfree/fedora/releases/43/Everything/x86_64/os/repoview/broadcom-wl.html and …/akmod-wl.html
- `kmod-wl` exists per a guide. [S] https://gist.github.com/bentoro/19f49d42265bf24c17652d8bbd74f628
- RPM Fusion's own Broadcom how-to page: NOT FOUND (404).

**What akmods does** [C] https://rpmfusion.org/Howto/NVIDIA and https://rpmfusion.org/Howto/Secure%20Boot
- At boot it checks for missing kmods and rebuilds them locally. This needs gcc and similar tools; the NVIDIA build can take up to 5 minutes.
- Fedora 36+: it signs locally built modules with a self-generated key (`kmodgenca -a`), stored in `/etc/pki/akmods`. You enrol the public key with `mokutil --import`.
- Rebuilds run through `akmods@.service`, which the NVIDIA page names in its initramfs workaround.
- On OSTree systems, signing needs a hack: a separate package that carries the keys. [C] NVIDIA page; example https://github.com/CheariX/silverblue-akmods-keys
