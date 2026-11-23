# L4: How Firefox picks a language

*Research by a Claude research agent, 2026-11-22, run at the lead's request; copied here as returned. Not re-checked line by line.*

**Answer:** Firefox's own menus and its language header are set separately. Menus follow `intl.locale.requested`, but only for languages that have a language pack installed. Fedora installs language packs automatically to match the system language. YouTube's help says its web language is chosen on the site and stored in the browser's cookies. Google doesn't say whether youtube.com/tv reads the browser's language header.

## 1. The two prefs (a "pref" is a Firefox setting)

- `intl.locale.requested` lists the languages wanted for Firefox's own interface, in order of preference. Left unset, Firefox uses its built-in default. Set to an empty string `""`, it follows the operating system's language. [C] [Mozilla locale docs](https://firefox-source-docs.mozilla.org/intl/locale.html)
- Firefox only uses a language that is "available", meaning built in or added by a language pack. [C] same page
- `intl.accept_languages` sets the `Accept-Language` header, the line in every web request that tells sites which languages the user wants. If nobody has set or locked it, Firefox works it out from the interface language. [C] [LocaleService.cpp](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/intl/locale/LocaleService.cpp), [nsHttpHandler.cpp](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/netwerk/protocol/http/nsHttpHandler.cpp), [all.js](https://raw.githubusercontent.com/mozilla-firefox/firefox/main/modules/libpref/init/all.js)

## 2. Language packs and policies

- A language pack is a kind of browser extension that holds only translations. [C] [locale docs](https://firefox-source-docs.mozilla.org/intl/locale.html)
- Fedora's `firefox-langpacks` package puts 114 of them in `/usr/lib64/firefox/langpacks/`. Fedora 45 stable carries version 160.0-1. [C] [Fedora package page](https://packages.fedoraproject.org/pkgs/firefox/firefox-langpacks/fedora-45.html)
- The `/usr/bin/firefox` launch script reads the system language from `LC_ALL`, `LC_MESSAGES` or `LANG`. It then links the matching pack into `~/.mozilla/extensions/…`, which every profile of that user shares. `MOZ_DISABLE_LANGPACKS=1` turns this off. [S] The agent read CentOS Stream's copy of the script, because Fedora's server blocked automated access: [firefox.sh.in](https://gitlab.com/redhat/centos-stream/rpms/firefox/-/raw/c10s/firefox.sh.in)
- The `RequestedLocales` policy sets the language list and "will cause the corresponding language pack to become active". It has existed since Firefox 64. [C] [RequestedLocales](https://firefox-admin-docs.mozilla.org/reference/policies/requestedlocales/)
- The `ExtensionSettings` policy can install language packs from Mozilla's release server or from a `file:///` path. [C] [ExtensionSettings](https://firefox-admin-docs.mozilla.org/reference/policies/extensionsettings/)
- The `Preferences` policy does not accept `intl.accept_languages`. Its only allowed `intl.` setting is `intl.keyword.enabled`. [C] [Preferences](https://firefox-admin-docs.mozilla.org/reference/policies/preferences/)

## 3. Do sites follow the header?

- YouTube help: "The language settings are saved in the browser". Clearing cookies means setting the language again. [C] [YouTube help 87604](https://support.google.com/youtube/answer/87604?hl=en)
- YouTube's TV apps "follow the language and location settings of your device". [C] same page
- Whether youtube.com/tv in a browser reads `Accept-Language` or the Google Account language: NOT FOUND. [Google's account help](https://support.google.com/accounts/answer/32047?hl=en) doesn't mention YouTube.

**Recommendation (the research agent's, not a decision):** For English only, set `RequestedLocales` to `["en-US"]`. Either remove `firefox-langpacks` or launch with `MOZ_DISABLE_LANGPACKS=1`, so the system language can't override the setting.

## Not confirmed

- Fedora 45's launch script is the same as CentOS Stream's.
- Whether the launch script's language pack overrides `RequestedLocales`. Needs testing.
- What youtube.com/tv, Twitch and Dropout use to pick a language: the header, cookies or the account.
- Whether `intl.accept_languages` can be locked another way, through `user.js` (a settings file in the profile) or autoconfig (Firefox's admin settings file).
- The Fedora change that made `firefox-langpacks` an optional, separate package; seen only in search results.
