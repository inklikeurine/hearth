# G1: How TV platforms give web apps TV features

*Research by a Claude research agent, 2026-11-22, run at the lead's request; copied here as returned. Not re-checked line by line.*

**Answer:** Neither platform gives third-party web apps much. Both send remote keys as ordinary DOM `keydown` events, which are the standard browser key events. Back is a key code the app handles itself. Neither offers a light or dark theme query: NOT FOUND on either site. Samsung lets apps read one accessibility setting, high contrast. LG keeps toasts and alerts for system apps only. On both, the app draws its own exit popup.

| Platform | API | What it does | Link |
|---|---|---|---|
| webOS | History API (default) | Back fires `popstate` while the app's history has entries. At the entry page, Back goes to Home. [C] | [Back Button](https://webostv.developer.lge.com/develop/guides/back-button) |
| webOS | `disableBackHistoryAPI: true` in appinfo.json | The app gets Back as `keydown` with code 461 (0x1CD) and handles it itself. [C] | same |
| webOS | `webOS.platformBack()` | Acts like the remote's Back key. On webOS 6.0+ it shows the system's exit popup; on 5.0 and older it opens Home. [C] | [webOS API](https://webostv.developer.lge.com/develop/references/webostvjs-webos) |
| webOS | `webOS.keyboard.isShowing()`, `keyboardStateChange` event | Tells the app whether the on-screen keyboard is visible. [C] | same; [webOS Events](https://webostv.developer.lge.com/develop/references/webos-event) |
| webOS | `webOSLaunch`, `webOSRelaunch`, `webOSLocaleChange`, `cursorStateChange`, `webOSMouse` | Lifecycle, language-change and pointer events. [C] | [webOS Events](https://webostv.developer.lge.com/develop/references/webos-event) |
| webOS | window `focus`/`blur` | How an app notices that a system overlay (Launcher, Settings) has taken over. There is no overlay API. [C] | [System UI Visibility](https://webostv.developer.lge.com/develop/guides/system-ui-visibility) |
| webOS | `luna://com.webos.settingsservice/getSystemSettings` | Reads locale, country, audio guidance and caption settings, with change subscription. It has no theme or high-contrast key. [C] | [Settings Service](https://webostv.developer.lge.com/develop/references/settings-service) |
| webOS | Toasts and alerts | "Only available in system apps, not 3rd party apps." [C] | [Notifications](https://webostv.developer.lge.com/develop/guides/notifications) |
| Tizen | `tizen.tvinputdevice`: `getSupportedKeys`, `getKey`, `registerKey`, `unregisterKey`, `registerKeyBatch`, `unregisterKeyBatch` | The app opts in to extra keys (media, colour, volume). Arrows, Enter and Back (10009) always arrive and cannot be registered. Needs the `tv.inputdevice` privilege. [C] | [TVInputDevice API](https://developer.samsung.com/smarttv/develop/api-references/tizen-web-device-api-references/tvinputdevice-api.html); [Remote Control](https://developer.samsung.com/smarttv/develop/guides/user-interaction/remote-control.html) |
| Tizen | Back policy + `tizen.application.getCurrentApplication().exit()` | Back must go to the previous page. On the home page it must open an exit popup the app builds in HTML. Long-press Return forces the app to quit, so apps must not register it. [C] | [Terminating Applications](https://developer.samsung.com/smarttv/develop/guides/fundamentals/terminating-applications.html) |
| Tizen | `webapis.tvinfo.getMenuValue()` | Reads `ACCESSIBILITY_HIGH_CONTRAST`, `ACCESSIBILITY_FOCUS_ZOOM`, `VOICE_GUIDE_KEY`, `UI_SIZE`, `UI_TRANSPARENCY`, `MOVING_TEXT_AND_IMAGE` and caption keys. The change listener is documented for caption keys only. [C] | [TvInfo API](https://developer.samsung.com/smarttv/develop/api-references/samsung-product-api-references/tvinfo-api.html) |
| Tizen | TV Window | Controls the main and picture-in-picture video windows. It does not draw system UI. [C] | [API index](https://developer.samsung.com/smarttv/develop/api-references/tizen-web-device-api-references.html) |
| Tizen | Toast or system UI API | NOT FOUND. Push is listed but marked unsupported on TVs. [C] | same |

**Recommendation (the research agent's, not decided):** Both platforms use the same pattern, which fits Hearth:
- Back is a normal key event the app can handle.
- A platform call gives the system's default Back action (`platformBack`).
- Settings are read-only, and the app can subscribe to changes.

Neither lets apps put up system overlays, so a Hearth overlay-request API would go further than either platform.

## Not confirmed

- Whether `getMenuValue(ACCESSIBILITY_HIGH_CONTRAST)` returns 0/1 or a string. Samsung's FAQ shows `"true"` for voice guide. This needs a real TV.
- The full webOS key code list (the Magic Remote guide was not read).
- Whether Tizen `hide()` is the right call for sending an app to the background. The Multitasking guide was not read.
- Whether webOS has a theme or high-contrast setting beyond the documented keys.
