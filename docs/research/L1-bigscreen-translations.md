# L1: How Bigscreen's text gets translated

*Research by a Claude research agent, 2026-11-22, run at the lead's request; copied here as returned. Not re-checked line by line.*

**Answer:** Bigscreen uses KDE's `i18n()` everywhere and never uses Qt's `qsTr()`. Its translations are gettext catalogs (`.mo` files, the compiled form of the `.po` text files translators edit), installed under `/usr/share/locale/`. KDE's volunteer translation teams produce them, and they are built into the package at build time. The device needs no network access to use them.

## 1. i18n() or qsTr()

- 60 QML files call `i18n()` (or `i18nc`, `i18np`, `i18nd`, `i18ndc`, `i18ndp`). There are 0 hits for `qsTr`, `qsTranslate` or `qsTrId`. Checked by grepping the master tarball. [C] https://github.com/KDE/plasma-bigscreen
- Examples: `kcms/wifi/ui/main.qml:26` has `title: i18n("Network")`, and `containments/homescreen/package/contents/ui/indicators/Settings.qml:18` has `text: i18n("Settings")`. [C]
- Each component has a `Messages.sh` that runs `$XGETTEXT` (KDE's wrapper for the gettext `xgettext` tool) over its `.cpp` and `.qml` files to make a template such as `kcm_mediacenter_wifi.pot`. [C] (same repo)

## 2. Loading translations without Plasma

- Bigscreen's standalone apps call `KLocalizedString::setApplicationDomain("plasma-bigscreen-settings")` after creating the app. They then call `KLocalization::setupLocalizedContext(&engine)` and/or `setContextObject(new KLocalizedContext(&engine))`. A "domain" is the catalog's name. Files: `settingsapp/main.cpp`, `linkredirector/main.cpp`, `uvcviewer/src/main.cpp`. [C]
- `KLocalizedContext` has been deprecated since KDE Frameworks 6.8, with the message "use KLocalizedQmlContext or KLocalization::setupLocalizedContext() from KF6::I18nQml instead". [C] https://github.com/KDE/ki18n/blob/master/src/i18n/klocalizedcontext.h, https://api.kde.org/klocalizedqmlcontext.html
- Files needed at runtime:
  - the KF6 I18n and I18nQml libraries [C, names from the deprecation message]
  - the catalogs `<datadir>/locale/<lang>/LC_MESSAGES/<domain>.mo`, which KI18n finds through `QStandardPaths::locate(GenericDataLocation, "locale/…")` [C] https://github.com/KDE/ki18n/blob/master/src/i18n/kcatalog.cpp
- The language comes from the `LANGUAGE`, `LC_*` and `LANG` environment variables. [C] https://github.com/KDE/ki18n/blob/master/docs/programmers-guide.md
- The homescreen QML is a Plasma package loaded by plasmashell. Plasma sets its translation domain automatically, so outside Plasma Hearth would have to set that domain itself. [M]

## 3. Where the translations come from, and network use

- KDE translation teams work on the files in KDE's Subversion server (SVN). A bot, "l10n daemon script" (known as scripty), commits the `.po` files into each repo's `po/` folder, with commits titled "Sync po/docbooks with svn". [C] (commit history of `po/de/plasma-bigscreen-settings.po`)
- Bigscreen's `po/` folder has 53 languages. [C]
- KDE started this nightly copying into repositories in 2022. [S] https://mail.kde.org/pipermail/kde-devel/2022-September/001268.html
- The templates (`.pot` files) moved to git in November 2025. [S] https://mail.kde.org/pipermail/kde-devel/2025-November/003911.html
- `ki18n_install(po)` in `CMakeLists.txt` compiles and installs the catalogs at build time. [C]
- Nothing fetches translations at runtime. KI18n only reads local files. [C, kcatalog.cpp]

**Recommendation (the research agent's, not a decision):** Borrowed Bigscreen QML keeps its `i18n()` calls. Choosing KI18n for Hearth means adding a KF6 dependency. Choosing `qsTr()` means rewriting the borrowed strings and losing KDE's existing translations. This choice is the lead's.

## Not confirmed

- Whether translators' `.po` files have also left SVN after November 2025.
- How Plasma sets the domain for packages (from memory only).
- Which Fedora 45 package ships Bigscreen's `.mo` files, and whether it splits them by language.
- Whether KI18n pulls in other runtime data, such as iso-codes for language names.
- Whether translations load correctly in a labwc session without plasmashell. This needs testing.
