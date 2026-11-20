# C2 — KDE Connect: QR pairing and older protocol versions
 
Tags: [C] = read in the primary source linked. [S] = secondary. No [M] claims.
 
## 1. Pairing by scanning a QR code: No
 
- Searching every file of the [desktop](https://github.com/KDE/kdeconnect-kde), [Android](https://github.com/KDE/kdeconnect-android) and [iOS](https://github.com/KDE/kdeconnect-ios) repos (master, 20 Nov 2026) for "QR" and "barcode" finds nothing. [C]
- Open feature request for QR pairing: NOT FOUND. Closest open bugs, neither about pairing:
  - [Bug 411731](https://bugs.kde.org/show_bug.cgi?id=411731): scan a Wi-Fi barcode to join a network. [C]
  - [Bug 469874](https://bugs.kde.org/show_bug.cgi?id=469874): sharing between unpaired devices; mentions a short verification string, not QR. [C]
- A forum post asks for it, but no bug was filed by its author: [discuss.kde.org](https://discuss.kde.org/t/how-to-robustly-connect-to-kde-connect-across-different-networks-will-scan-to-connect-to-kde-connect-be-a-good-idea/46292). [S]
- bugs.kde.org's own search is blocked to my tools, so a request could exist that I missed.
## 2. Desktop refusing older protocol versions: No setting (NOT FOUND)
 
No protocol option exists in the desktop app's UI or config files. [C] What the code does:
 
- **Released, v26.08.2:** refuses only a *paired* device that falls below the protocol version it used before ([call](https://github.com/KDE/kdeconnect-kde/blob/v26.08.2/core/backends/lan/lanlinkprovider.cpp#L552), [function](https://github.com/KDE/kdeconnect-kde/blob/v26.08.2/core/backends/lan/lanlinkprovider.cpp#L577)). An unpaired older device only triggers a log warning ([device.cpp](https://github.com/KDE/kdeconnect-kde/blob/v26.08.2/core/device.cpp#L72)). [C]
- **Beta tag v26.11.80:** a fixed refusal of any LAN device below protocol 8 ([code](https://github.com/KDE/kdeconnect-kde/blob/v26.11.80/core/backends/lan/lanlinkprovider.cpp#L552)), added 13 Sep 2026 by commit [94b1f0a](https://github.com/KDE/kdeconnect-kde/commit/94b1f0a), "Drop support for protocol v7". Not switchable. [C]
- Not in v26.08.3. [C]
- Bluetooth backend: no version check found. [C]
- Fedora 45's metadata lists 26.08.2 (local file `fedora45-primary.xml.zst`, no web link), so it lacks the fixed floor.
 
