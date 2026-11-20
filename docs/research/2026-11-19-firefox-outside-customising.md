# How far stock Firefox can be changed from outside (2026-11-19)

Research task F1, answered in a design session from Firefox's own source and Mozilla's admin docs instead of a Sonnet chat. Tags: [C] read in Firefox source (github.com/mozilla-firefox/firefox, main branch, 2026-11-19) or Mozilla's own docs; [S] third-party project; [M] inference, not confirmed. Mozilla's support site (support.mozilla.org) blocked automated reading, so its kiosk, AutoConfig and userChrome articles were not read.

## Answer

Stock Firefox can be changed far enough that a fork is probably unnecessary. The reason is AutoConfig without its sandbox: a startup script with full browser privileges, shipped in the install folder, with no rebuild. Its cost is that it changes Firefox's internals, which can break on Firefox updates. Hearth pins and tests the Firefox version in the image (design doc, section 2), so a break is caught before it ships [M].

| Method | What it can change | Status | Source |
|---|---|---|---|
| Enterprise policies (`policies.json`) | Lock down the browser: hide menu and bookmarks bars, block `about:config` / `about:addons` / developer tools, force-install and lock extensions (`ExtensionSettings`), turn off first-run and update pages, permissions (autoplay, notifications), website filter, DNS-over-HTTPS, DRM downloads, set and lock prefs (`Preferences`, limited to listed prefixes such as `browser.`, `ui.`, `widget.`, `media.`, `dom.`, `network.`, `layout.`, `extensions.`). No policy for keyboard shortcuts, fullscreen or the tab strip | Supported. Read from `distribution/` in the install folder or `/etc/firefox/policies` on Linux | [C] [policy templates](https://mozilla.github.io/policy-templates/) (page warns it is no longer current), [admin docs policy index](https://firefox-admin-docs.mozilla.org/reference/policies/) |
| Kiosk mode (`--kiosk`) | Forces full screen and keeps it; the toolbars stay hidden and can't be brought back; `browser.kiosk.lockdown` (default 2) controls how strict. No sign in the code read that it changes keyboard shortcuts | Supported command-line flag (`--kiosk`, `--kiosk-monitor <n>`) | [C] `BrowserContentHandler.sys.mjs`, `browser-init.js`, `browser-fullScreenAndPointerLock.js` |
| `userChrome.css` / `userContent.css` | Restyle the browser's own interface and default page styles | Off by default: `toolkit.legacyUserProfileCustomizations.stylesheets = false`. The `Preferences` policy is allowed to set that pref | [C] `modules/libpref/init/all.js`, `Policies.sys.mjs`, policy templates |
| AutoConfig (`defaults/pref/autoconfig.js` + a `.cfg` file in the install folder) | Sandboxed: prefs only. Unsandboxed: full browser privileges (`Components`, `ChromeUtils`, `Services`), so it can change the browser window itself: remove or rebind built-in shortcuts, hide or add interface, load style sheets without the userChrome pref | Supported. Sandboxed by default on beta and release builds, but `pref("general.config.sandbox_enabled", false)` in `autoconfig.js` turns the sandbox off, and Firefox's own test checks that the script then gets full privileges | [C] `extensions/pref/autoconfig/src/nsReadConfig.cpp` (lines ~150–157), `test/unit/test_autoconfig_no_sandbox.js` |

A maintained community loader builds on unsandboxed AutoConfig to run browser-interface scripts: [fx-autoconfig](https://github.com/MrOtherGuy/fx-autoconfig) (MPL-2.0, last push 2026-07-23) [S].

## What this means for the open questions

- Remote keys: the browser's own Escape, Backspace and Back handling (design doc, section 6.2) can be rebound at browser level by an unsandboxed AutoConfig script; policies can't do it [C for the mechanism, M that it covers every key we need: test it].
- Fork (F2) and embedding (F3): the main reasons to fork (our own interface, key handling, lock-down) are reachable without one [M]. What stays out of reach: the page can't be an item inside Hearth's own Qt scene, so it can't be animated like a tile. Overlays on top of it are a compositor job (series E, prompt E1).
- Security: an unsandboxed script runs with the browser's full rights. It lives in the read-only OS image, written and reviewed by us, so it adds no outside attack route [M].
- Still to test: that unsandboxed AutoConfig works in the Firefox build the image ships, the exact keys it can rebind, and what kiosk mode does to context menus and dialogs (not visible in the code read).
