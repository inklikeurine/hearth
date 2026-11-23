# L3: System language settings and their size

*Research by a Claude research agent, 2026-11-22, run at the lead's request; copied here as returned. Not re-checked line by line.*

**Short answer:** If Hearth installs one glibc langpack per language, each adds about 4 to 16 MB. Installing all of them (glibc-all-langpacks) adds about 238 MB. systemd-localed keeps its settings as plain text files in `/etc`. Fedora's language meta-packages pull in fonts and extra add-on packages through weak dependencies. Most programs' own translations are not part of that mechanism.

Sizes are installed sizes in Fedora 45 (glibc 2.44-2.fc45), read from [mdapi](https://mdapi.fedoraproject.org/f45/pkg/glibc-all-langpacks), the Fedora package database's lookup service. The packages.fedoraproject.org pages list no sizes ([example](https://packages.fedoraproject.org/pkgs/glibc/glibc-langpack-en/)).

## 1. glibc langpacks

- A glibc langpack holds the locale data for one language: the rules for dates, numbers, sorting and character classes. glibc is the C library every Linux program uses. [C]
- `glibc-langpack-en`: 6.0 MB installed, 0.69 MB download ([mdapi](https://mdapi.fedoraproject.org/f45/pkg/glibc-langpack-en)). [C]
- `glibc-langpack-de`: 3.8 MB ([mdapi](https://mdapi.fedoraproject.org/f45/pkg/glibc-langpack-de)). `glibc-langpack-zh`: 15.6 MB ([mdapi](https://mdapi.fedoraproject.org/f45/pkg/glibc-langpack-zh)). [C]
- `glibc-all-langpacks`: 238 MB installed, 18.5 MB download. [C]
- `glibc-minimal-langpack`: 0 bytes. It is an empty meta-package (a package that only pulls in others) that limits the system to the C, POSIX and C.UTF-8 locales ([mdapi](https://mdapi.fedoraproject.org/f45/pkg/glibc-minimal-langpack)). [C]

## 2. systemd-localed (systemd 262 man pages)

- It sets three things: the system locale, the console keymap and the default X11 keyboard layout. `localectl` is the command you use to talk to it ([man](https://www.freedesktop.org/software/systemd/man/latest/systemd-localed.service.html)). [C]
- The locale goes in `/etc/locale.conf` as `LANG=` and `LC_*=` lines. systemd reads it at early boot and every service inherits it. It can be overridden at boot with `locale.LANG=` on the kernel command line ([man](https://www.freedesktop.org/software/systemd/man/latest/locale.conf.html)). [C]
- The console keymap goes in `/etc/vconsole.conf` as `KEYMAP=` ([man](https://www.freedesktop.org/software/systemd/man/latest/vconsole.conf.html)). [C]
- The X11 layout goes in two files: `XKBLAYOUT`, `XKBMODEL`, `XKBVARIANT` and `XKBOPTIONS` lines in `/etc/vconsole.conf`, and `/etc/X11/xorg.conf.d/00-keyboard.conf`. The man pages don't say this; the agent read it in [localed-util.c](https://github.com/systemd/systemd/blob/main/src/locale/localed-util.c) on systemd's main branch. [C]
- Unless you pass `--no-convert`, setting one keymap also converts and sets the other ([man](https://www.freedesktop.org/software/systemd/man/latest/localectl.html)). [C]

## 3. Fedora langpacks

- `langpacks-de` requires `langpacks-core-de` and `langpacks-fonts-de`. Core requires `default-fonts-de`. The fonts package adds Noto fonts ([mdapi](https://mdapi.fedoraproject.org/f45/pkg/langpacks-de)). [C]
- The pull runs in reverse. Add-on packages declare `Supplements:`, meaning "install me if these are present", so the meta-package itself lists none of them:
  - glibc-langpack-de: `(glibc and langpacks-core-de) unless glibc-all-langpacks`. [C]
  - hunspell-de: `(hunspell and langpacks-de)`, 13 MB ([mdapi](https://mdapi.fedoraproject.org/f45/pkg/hunspell-de)). [C]
- The packaging rule: translation-only subpackages are named `<pkg>-langpack-<code>` and carry `Supplements: (%{name} = %{version}-%{release} and langpacks-<code>)`. The package manager works this out itself, with no plugin ([wiki](https://fedoraproject.org/wiki/Packaging:Langpacks)). [C, wiki page marked "Fedora 23 and later"; the current docs.fedoraproject.org page blocked the agent's fetch]
- `langpacks-core-*` was added in Fedora 31 for minimal support: the default font, the locale, and an input method only where needed ([change](https://fedoraproject.org/wiki/Changes/Langpacks-core)). [C]
- Most programs ship their translation files (`.mo`) inside the main package. Langpacks don't control those. The rpm macro `%_install_langs` filters them at install time. [M]

**Recommendation (the research agent's, not a decision):** For English-only 1.0, install `glibc-langpack-en` plus `langpacks-core-en` and skip `glibc-all-langpacks`. That saves about 230 MB.

## Not confirmed

- Whether Fedora 45 or Rawhide changed the langpacks packaging guideline (docs.fedoraproject.org was blocked).
- How `%_install_langs` behaves under mkosi, and how much it saves.
- Whether labwc and the Hearth UI read the XKB layout from localed's files, or need environment variables.
- The size of the font packages behind `default-fonts-*`.
- localed writes to `/etc`, so `/etc` must be writable on the read-only image. Not checked how mkosi and systemd-sysupdate handle `/etc` for this.
