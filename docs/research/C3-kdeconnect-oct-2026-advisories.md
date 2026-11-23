# C3: KDE Connect advisories of October 2026

*Research by a Claude research agent, 2026-11-22, run at the lead's request; copied here as returned. Not re-checked line by line.*

**Answer:** KDE published two KDE Connect advisories on 30 Oct 2026. Both can be triggered by anyone on the same local network, and neither is a remote takeover. Fedora 45's kde-connect 26.08.3 is not affected by either. The desktop fix is in 26.08.3 itself, and the other bug affects only the Android app.

| Advisory | CVE | The bug | App | Affected versions | Fixed in | Fedora 45 status | Link |
|---|---|---|---|---|---|---|---|
| advisory-20261030-1 | CVE-2026-111431 | A packet containing deeply nested JSON (data structures packed many layers inside each other) crashes the app. Android restarts it, so the attack can be repeated to keep it down. [C] | Android [C] | KDE Connect Android < 1.37.0 [C] | 1.37.0. The advisory only says "< 1.37.0". The fix commit is in tag v1.37.0 [C] | Not applicable: this is the Android app, not the Fedora package [C] | [KDE](https://kde.org/info/security/advisory-20261030-1.txt), [commit tags](https://invent.kde.org/network/kdeconnect-android/-/commit/17778d6d47664e9836a65e6d02414adcf0baff74) |
| advisory-20261030-2 | CVE-2026-111426 | Every malformed packet made KDE Connect send a D-Bus signal (D-Bus is the message bus desktop programs use to talk to each other). A stream of bad packets floods it and slows or freezes KDE Connect, and with the Plasma widget also the desktop. [C] | Desktop (kdeconnect-kde) [C] | KDE Connect < 26.08.3 [C] | 26.08.3. The fix commit is in tag v26.08.3 [C] | Not affected. Bodhi update FEDORA-2026-78c0581f85 (kde-connect-26.08.3-1.fc45) went stable on 2026-11-15. It is filed as "enhancement", not "security", and does not mention the CVE [C] | [KDE](https://kde.org/info/security/advisory-20261030-2.txt), [Bodhi](https://bodhi.fedoraproject.org/updates/FEDORA-2026-78c0581f85) |

## Supporting notes

- KDE's index lists exactly these two KDE Connect advisories for 30 Oct 2026 [C] ([index](https://kde.org/info/security/)).
- Advisory 2 is about the desktop app (its patch is in kdeconnect-kde), but its "Solution" section says "Update KDE Connect Android". This looks like a copy-paste error in KDE's text [C].
- NVD has no record of either CVE yet. Its API returned 0 results for both on 2026-11-23 [C] ([NVD API](https://services.nvd.nist.gov/rest/json/cves/2.0?cveId=CVE-2026-111426)).
- Unrelated to these two: the earlier Fedora 45 security update FEDORA-2026-21f4914bca (kde-connect 26.08.2) lists CVE-2026-110236, "Arbitrary command execution via crafted folder name" [C] ([Bodhi](https://bodhi.fedoraproject.org/updates/FEDORA-2026-21f4914bca)). That update covers 331 packages and doesn't say which one the CVE belongs to. No KDE advisory for it is on KDE's index.

**Recommendation (the research agent's, not a decision):** keep kde-connect at 26.08.3 or later, and require KDE Connect Android 1.37.0 or later on phones.

## Not confirmed

- Whether KDE Connect for iOS is affected by either bug. Neither advisory mentions iOS: NOT FOUND.
- NVD severity scores for both CVEs: NOT FOUND (no NVD entries yet).
- Which package CVE-2026-110236 belongs to. A search summary claimed KDE Connect; not confirmed, and KDE's index has no matching advisory.
- Whether a real KDE Connect Android release older than 1.37.0 actually crashes. That needs testing on a real device.
