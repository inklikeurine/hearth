# L2: Font coverage and size per script

*Research by a Claude research agent, 2026-11-22, run at the lead's request; copied here as returned. Not re-checked line by line.*

**Answer:** Fedora 45 pages exist, but packages.fedoraproject.org does not show installed size for any package; both the index page and the release page have no size field [C] ([example](https://packages.fedoraproject.org/pkgs/google-noto-fonts/google-noto-sans-fonts/fedora-45.html)). The sizes below come from Fedora 45's own repository metadata: [primary.xml](https://dl.fedoraproject.org/pub/fedora/linux/releases/45/Everything/x86_64/os/repodata/), dated 2026-10-28, with checksum verified, field `size installed` [C]. "vf" means a *variable font*: one file holds every weight, so it is much smaller than the "static" package, which ships one file per weight.

| Script | Fedora package | Installed size | Link |
|---|---|---|---|
| Latin/Cyrillic/Greek | google-noto-sans-vf-fonts | 1.4 MiB [C] | [page](https://packages.fedoraproject.org/pkgs/google-noto-fonts/google-noto-sans-vf-fonts/) |
| same, static | google-noto-sans-fonts | 43.1 MiB [C] | [page](https://packages.fedoraproject.org/pkgs/google-noto-fonts/google-noto-sans-fonts/) |
| Arabic | google-noto-sans-arabic-vf-fonts | 236 KiB [C] (static: 8.3 MiB) | [page](https://packages.fedoraproject.org/pkgs/google-noto-fonts/google-noto-sans-arabic-vf-fonts/) |
| Hebrew | google-noto-sans-hebrew-vf-fonts | 27 KiB [C] (static: 962 KiB) | [page](https://packages.fedoraproject.org/pkgs/google-noto-fonts/google-noto-sans-hebrew-vf-fonts/) |
| Devanagari | google-noto-sans-devanagari-vf-fonts | 280 KiB [C] (static: 8.3 MiB) | [page](https://packages.fedoraproject.org/pkgs/google-noto-fonts/google-noto-sans-devanagari-vf-fonts/) |
| Thai | google-noto-sans-thai-vf-fonts | 33 KiB [C] (static: 1.3 MiB) | [page](https://packages.fedoraproject.org/pkgs/google-noto-fonts/google-noto-sans-thai-vf-fonts/) |
| CJK | google-noto-sans-cjk-vf-fonts | 31.2 MiB [C] (static google-noto-sans-cjk-fonts: 124.6 MiB) | [page](https://packages.fedoraproject.org/pkgs/google-noto-cjk-fonts/google-noto-sans-cjk-vf-fonts/) |
| Atkinson | atkinson-hyperlegible-next-fonts 2.100 | 592 KiB [C] | [page](https://packages.fedoraproject.org/pkgs/atkinson-hyperlegible-next-fonts/atkinson-hyperlegible-next-fonts/) |

Every Noto package also pulls in google-noto-fonts-common (18 KiB) [C].

**Fedora's default font set.** Fedora documents one: the `default-fonts` metapackages, introduced in Fedora 39 [C] ([Change page](https://fedoraproject.org/wiki/Changes/ImproveDefaultFontHandling)). A metapackage is a package with no files of its own that only pulls in other packages. In Fedora 45, the per-language metapackages for Arabic, Hebrew, Hindi, Thai and CJK sans each pull in exactly the vf package in the table above [C]. Totals, summed by the agent from the metadata, following dependencies:
- `default-fonts` (everything): 138 packages, 146.3 MiB [C]
- `default-fonts-core` (Western sans/serif/mono, emoji, maths): 13.6 MiB [C]
- `default-fonts-cjk` (sans, serif, mono): 116.5 MiB; CJK sans alone 31.2 MiB [C]
- `default-fonts-other` (other scripts): 19.9 MiB [C]

**Recommendation (the research agent's, not decided):** Use the vf packages. Every non-CJK script in the table plus Atkinson comes to about 2.6 MiB. CJK sans is the one big cost, at 31 MiB.

## Not confirmed

- Whether the vf packages contain every glyph and weight of the static ones. The Hebrew and Thai vf packages are surprisingly small. Check with `fc-list` on a real image.
- Whether mkosi or dnf install weak dependencies that make these totals larger.
- How much RAM fontconfig and rendering use at runtime. Test on hardware.
- MiB figures are converted from bytes (1 MiB = 1,048,576 bytes). They show unpacked disk use, not compressed image size.
