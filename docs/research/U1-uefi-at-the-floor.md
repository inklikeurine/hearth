# U1 — UEFI at the hardware floor
 
**Answer:** Yes, floor-era machines ran UEFI. Intel's plan to drop the old-BIOS layer (CSM) targeted 2020 platforms, after the floor. At the floor, CSM exists and users can switch it off.
 
## 1. UEFI and Intel's CSM plan
- Windows 8 (2012) rule: "All Windows 8 Client systems must ship in native UEFI mode" (Class 2 with CSM disabled, or Class 3). [C] [Intel IDF 2012, slide 20](https://intel.com/content/dam/develop/external/us/en/documents/sf12-efis004-100-820238.pdf)
- Classes: 0 = legacy BIOS only; 1 = UEFI code but only legacy interfaces exposed; 2 = both (CSM on); 3 = UEFI only. [C] [Intel talk at UEFI Forum Plugfest, Oct 2017, slides 7–8](https://uefi.org/sites/default/files/resources/Brian_Richardson_Intel_Final.pdf)
- Same talk, slide 15: Intel plans "removing legacy BIOS support from client & data center platforms by 2020." Those platforms become Class 3, with no 16-bit option ROMs. It was a plan, not a confirmed result. [C] same link
- Intel's NUC page: legacy BIOS dropped on "some 10th generation" platforms from 2020. [C] [Intel](https://intel.com/content/www/us/en/support/articles/000057401.html)
- Reading (mine, not a source's): Kaby Lake (2017) and Raven Ridge (2018) predate the cut, so CSM is likely still present. Recommendation: treat both CSM-on and CSM-off as real cases.
- A source stating the UEFI class of specific 2017–2018 boards: NOT FOUND.
## 2. Is legacy mode a switch?
- Dell, 7th-gen-and-newer systems: in F2 setup, set Boot Sequence from Legacy to UEFI and turn off "Enable Legacy Option ROMs." Dell says these systems are "designed to boot exclusively" in UEFI. If the OS was installed in Legacy mode, switching back doesn't fix it; reinstall. [C] [Dell KB 000151899](https://dell.com/support/kbdoc/en-ie/000151899/no-boot-device-found-when-you-change-the-boot-sequence-to-legacy-mode)
- AMD, Ryzen with Radeon Vega (Raven Ridge class): set "Compatibility Support Module" or "CSM" to disabled, usually under Boot. [C] [AMD](https://www.amd.com/en/resources/support-articles/faqs/CPU-UEFI-Mode.html)
- Forum reports of AM4 boards that won't disable CSM because the GPU lacks a UEFI display driver. [S] [ASRock forum](https://forum.asrock.com/printer_friendly_posts.asp?TID=18824)
## Not confirmed
- HP and Lenovo pages (not searched).
- Whether Hearth's installer boots with CSM on and off on real Kaby Lake and Raven Ridge boards: needs hardware testing.
 
