# F4: Firefox memory settings

*Research by a Claude research agent, 2026-11-22, run at the lead's request; copied here as returned. Not re-checked line by line.*

**Answer:** The settings that save the most memory are the ones that cut how many processes Firefox runs. With one site open, Fission (site isolation, which puts each website in its own operating-system process) costs little, because it mostly adds processes for embedded third-party frames. Turning Fission off has a real security cost. No Mozilla measurement of RAM with one YouTube tab was found.

Defaults below are from Firefox source `main`, which is 163.0a1 [C] ([version.txt](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/browser/config/version.txt)). They may differ in Fedora's Firefox 160.

| Pref | What it does | Default | Security or compatibility cost | Link |
|---|---|---|---|---|
| `dom.ipc.processCount` | Maximum number of shared "web" content processes | 8 [C] | Only matters if Fission is off. Fewer processes means a crash or hang takes down more pages at once [M] | [all.js](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/modules/libpref/init/all.js) |
| `dom.ipc.processCount.webIsolated` | Maximum processes per site when Fission is on | 4 on desktop [C] (Mozilla's process-model doc still says 1 [C]) | Setting it to 1 puts all frames from one site in one process. Bug 1727158 found mixed memory results and a performance regression in the change from 1 to 4 [C] | [all.js](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/modules/libpref/init/all.js), [doc](https://firefox-source-docs.mozilla.org/dom/ipc/process_model.html), [bug 1727158](https://bugzilla.mozilla.org/show_bug.cgi?id=1727158) |
| `dom.ipc.processPrelaunch.enabled` / `.fission.number` | Starts spare content processes ahead of time | true / 3 [C]; spares are closed on memory pressure [C] | Navigating to a new site is slower. No security cost [M] | [firefox.js](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/browser/app/profile/firefox.js), [StaticPrefList](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/modules/libpref/init/StaticPrefList.yaml), [source](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/dom/ipc/PreallocatedProcessManager.cpp) |
| `fission.autostart` | Turns site isolation on or off | true [C] | Weaker protection against Spectre (attacks that read another site's memory through CPU flaws) [C]. Isolated processes skip some JavaScript Spectre mitigations [C], so turning Fission off probably makes JavaScript slower [M] | [blog](https://blog.mozilla.org/security/2021/05/18/introducing-site-isolation-in-firefox/), [StaticPrefList](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/modules/libpref/init/StaticPrefList.yaml) |
| `fission.webContentIsolationStrategy` | 0 = isolate nothing, 1 = isolate every site, 2 = isolate only "high-value" sites | 1 [C] | 0 or 2 means weaker isolation | [ProcessIsolation.cpp](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/dom/ipc/ProcessIsolation.cpp) |
| `browser.sessionhistory.max_total_viewers` | Number of pages kept in memory for fast Back/Forward | -1 = based on RAM, which gives 8 at 4 GB [C] | Back is slower | [nsSHistory.cpp](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/docshell/shistory/nsSHistory.cpp) |
| `browser.cache.memory.capacity` | Size of the in-memory network cache | -1 = automatic [C] | More network re-fetches | [StaticPrefList](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/modules/libpref/init/StaticPrefList.yaml) |
| `media.mediasource.eviction_threshold.video` / `.audio` | How much streaming video and audio is buffered before old data is dropped | 150 MB / 20 MB [C] | Less buffer ahead, so possibly more stalls or errors on YouTube [M] | [TrackBuffersManager.cpp](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/dom/media/mediasource/TrackBuffersManager.cpp) |
| `image.mem.surfacecache.max_size_kb` | Limit on the cache of decoded images | about 2 GB [C] | More image re-decoding | [StaticPrefList](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/modules/libpref/init/StaticPrefList.yaml) |
| `browser.tabs.unloadOnLowMemory` | Unloads background tabs when memory is low | false on Linux [C] | Not relevant with one tab | [firefox.js](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/browser/app/profile/firefox.js) |

**Recommendation (the research agent's, not a decision):** Leave Fission on, turn prelaunch off, cap `webIsolated` at 1, and measure before changing anything else.

**Published measurements:** NOT FOUND for a measured one-YouTube-tab figure on a stated machine. The closest:
- Mozilla's per-content-process overhead on Linux was 19.5 MB in 2019. The page doesn't say who measured or on what machine [C] ([wiki](https://wiki.mozilla.org/Project_Fission/Memory)).
- In bug 1833296 (Firefox 113, Windows 10, RAM not stated), a Mozilla developer read the largest content process in a YouTube session at about 300 MB [C] ([bug 1833296](https://bugzilla.mozilla.org/show_bug.cgi?id=1833296)).
- Mozilla's tracking bug 1931717 records YouTube memory leaks reaching several GB over long sessions, with no fix yet. That is a risk for an appliance that stays on [C] ([bug 1931717](https://bugzilla.mozilla.org/show_bug.cgi?id=1931717)).

## Not confirmed

- The defaults in Fedora's Firefox 160 build.
- Whether `max_total_viewers` still applies when the Back/Forward cache lives in the parent process (`fission.bfcacheInParent`).
- How much each pref saves on YouTube TV, Twitch and Dropout. This needs testing on 4 GB hardware.
- Whether YouTube TV loads cross-site frames, which is what decides Fission's cost.
