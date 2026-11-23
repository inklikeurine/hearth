# D4: NVIDIA's supported GPUs and licence

*Research by a Claude research agent, 2026-11-22, run at the lead's request; copied here as returned. Not re-checked line by line.*

**Answer.** NVIDIA's current Linux driver supports Turing (GTX 16 / RTX 20) and newer cards only. Maxwell, Pascal and Volta stay on the older 580 branch. NVIDIA promises security fixes for those three until October 2028, but that promise is worded for GeForce cards in general and never mentions Linux. The licence lets the user install the driver. It also lets anyone pass the driver on unchanged with the licence attached. It says nothing about software downloading the driver for the user.

## 1. Supported generations

Current Linux driver: `latest.txt` points to 620.70.06 [C] ([latest.txt](https://download.nvidia.com/XFree86/Linux-x86_64/latest.txt)). Its supported-GPU list starts at Turing and has no GTX 10xx, GTX 9xx or Titan V cards. Those three appear under "The 580.xx driver supports the following set of GPUs:" [C] ([620.70.06 README, Appendix A](https://download.nvidia.com/XFree86/Linux-x86_64/620.70.06/README/supportedchips.html)).

| Generation | Example cards | Current branch? | Last branch that supports it | Link |
|---|---|---|---|---|
| Maxwell | GTX 750 Ti [M], GTX 980 [C] | No | 580 [C] | [deprecation schedule](https://forums.developer.nvidia.com/t/unix-graphics-feature-deprecation-schedule/60588) |
| Pascal | GTX 1050 [M], GTX 1080 [C] | No | 580 [C] | same |
| Volta | Titan V [C] | No | 580 [C] | same |
| Turing | GTX 1650, RTX 2080 Ti [C] | Yes (620) | Still current | [README](https://download.nvidia.com/XFree86/Linux-x86_64/620.70.06/README/supportedchips.html) |

- The NVIDIA staff post by aplattner, edited 1 July 2025, says: "The release 580 series will be the last to support GPUs based on the Maxwell, Pascal, and Volta architectures." [C] ([forum](https://forums.developer.nvidia.com/t/unix-graphics-feature-deprecation-schedule/60588)). That post does not say how long the 580 branch will last.
- The newest 580 Linux driver on NVIDIA's download server is 580.196.07 [C] ([archive](https://download.nvidia.com/XFree86/Linux-x86_64/)). That shows 580 is still getting updates.
- NVIDIA's news post says these GPUs "will transition to receiving quarterly security updates for the next three years (through October 2028)" [C] ([NVIDIA news](https://www.nvidia.com/en-us/geforce/news/mafia-the-old-country-geforce-game-ready-driver/)). The post does not mention Linux.
- NVIDIA's support pages give the same October 2028 date for GeForce and Quadro cards (Quadro is NVIDIA's workstation line). The agent could not open them (error 403), so this comes from search-result text only [S] ([GeForce plan](https://nvidia.custhelp.com/app/answers/detail/a_id/5676), [Quadro plan](https://nvidia.custhelp.com/app/answers/detail/a_id/5706)).

## 2. Licence

Source: the NVIDIA Driver License Agreement (v. February 25, 2025), which sits next to the 620.70.06 driver [C] ([license.txt](https://download.nvidia.com/XFree86/Linux-x86_64/620.70.06/license.txt)).

- **Install:** Section 1.1(a): "Install and use copies of the SOFTWARE". The licensee is "you, whether an individual or entity". The licence also says "if you don't accept all the terms below, do not use the SOFTWARE." [C]
- **Automatic download and install by software:** NOT FOUND. The licence neither permits nor forbids it.
- **Third-party redistribution:** allowed, with conditions. Section 1.1(d): "Distribute the SOFTWARE provided for use with operating system kernels distributed under the terms of an OSI-approved open source license… provided that (i) the binary files thereof are not modified in any way (except for uncompressing of compressed files) and (ii) this Agreement is provided to each SOFTWARE recipient." [C] Linux counts, since it is under the GPL, an OSI-approved licence [M].
- **Limits:** Section 2.7: "Except as expressly granted in this Agreement, you may not sell, rent, sublicense, distribute or transfer the SOFTWARE…" [C]. Section 2.3: "You may not modify or create derivative works of the SOFTWARE provided in binary form." [C]

**Recommendation (the research agent's):** at setup, show the user NVIDIA's licence and have them accept it before the driver is fetched. The user then becomes the licensee. The licence does not demand this step, but it makes Hearth's position clearer.

## Not confirmed

- Whether the October 2028 security promise covers the Linux 580 driver. NVIDIA's own statement is GeForce-wide and never says Linux.
- The exact wording of the two support pages (403 error).
- Whether 620.70.06 is NVIDIA's "production" or "new feature" branch. The download page shows no version labels.
- Whether RPM Fusion offers a 580 legacy package for Fedora 45. Not checked.
- The licence text of the 580 driver itself; only the one shipped with 620.70.06 was read.
