# C1 — KDE Connect 2025 security bugs
 
Tags: [C] = read in the primary source linked. No [S] or [M] claims below.
 
| CVE | What the bug was | App | Fixed in | KDE advisory | NVD |
|---|---|---|---|---|---|
| CVE-2025-32898 | The 8-character pairing code was short enough to brute-force a matching key pair and impersonate a device. [C] | Desktop, Android, iOS [C] | Desktop 25.04, Android 1.33.0, iOS 0.5 [C] | [advisory](https://kde.org/info/security/advisory-20250418-3.txt) | [NVD](https://nvd.nist.gov/vuln/detail/CVE-2025-32898) |
| CVE-2025-32900 | A forged UDP broadcast could temporarily change another device's displayed name or type. [C] | Desktop, Android, iOS [C] | Desktop 25.04, Android 1.33.0, iOS 0.5 [C] | [advisory](https://kde.org/info/security/advisory-20250418-2.txt) | [NVD](https://nvd.nist.gov/vuln/detail/CVE-2025-32900) |
| CVE-2025-66270 | Protocol 8 never checked that the device ID matched across two handshake packets, so a paired device could be impersonated. Rated Critical. [C] | Desktop, Android, iOS [C] | Desktop 25.12, Android 1.34.4, iOS 0.5.4 (note 2) | [advisory](https://kde.org/info/security/advisory-20251128-1.txt) | [NVD](https://nvd.nist.gov/vuln/detail/CVE-2025-66270) |
 
## Notes
 
1. The April fixes introduced protocol 8. CVE-2025-66270 affects only protocol 8: desktop 25.04–25.11, Android 1.33.0–1.34.3, iOS 0.5.2–0.5.3. [C, advisory]
2. That advisory gives affected ranges, not "fixed in"; fixed versions are the first version past each range. The desktop fix rejects a changed device ID mid-handshake. [C, [commit](https://github.com/KDE/kdeconnect-kde/commit/4e53bcdd5d4c28bd9fefd114b807ce35d7b3373e)]
3. Fedora 45 repo metadata lists `kde-connect` 26.08.2, past all three fixes. [C, local file `fedora45-primary.xml.zst`, no web link; snapshot date unconfirmed]
## Not confirmed
 
- Release dates of 25.12, 1.34.4, 0.5.4: NOT FOUND.
- invent.kde.org commit page wouldn't load; read the GitHub mirror. Android and iOS commits unread.
- NVD page for 66270 wouldn't render; used NVD's API record.
- Two later KDE Connect advisories (30 Oct 2026) are out of scope and unread.
## Recommendation (Claude's, not a decision)
 
Require desktop 25.12 or later in the Hearth image.
 
