# V1 – The lead's visual references and UI calls (2026-11-25)

Summarised from the lead's 11 reference screenshots (TV launchers, profile pickers, menus, an on-screen keyboard) and review comments in a claude.ai design session. The round 1 mockups (home screen, profile picker, settings, the menu over the home screen and over an app) are on a private claude.ai design canvas.

## The lead likes
- Menu overlay slides in from the left edge, docked full height; Settings toggle to swap it to the right.
- Home: 6 app tiles per row; 1 row by default, more rows as an option; wide tiles with room for logo and text. Clock top right.
- Controls (settings, profile, etc.) top right, with date and time just under them.
- The next row peeking in at the bottom, without a row heading.
- Showing which profile is signed in by its picture. Where it goes is undecided (not top left).
- Profile picker: clean and spare (YouTube's and Apple TV's); Guest on the picker; an "Edit" control appears under the highlighted profile (press Down); a background, themeable separately from the home background.
- Disney+-style avatar picker (rows of round avatars by group).
- Blur, transparency, slick human-scale animation.
- Widgets above the app rows as a possible future, not core; weather (today and next 5 days) as the example.
- Menu items: roomy spacing, an icon beside each label; shadow only on the edge facing into the screen.
- Settings background fading gradually into the screen.
- On-screen keyboard reference: an alphabet grid on the left, the field on the right.
- Steam Big Picture's side menu, generally.

## The lead dislikes
- A navigation rail on the left; big hero content in the middle of the home screen (reads as an ad); category tabs along the top. Content lives in apps.
- Brand logos and "Add account" on the profile picker.
- A menu floating in space, not docked to a screen edge.
- Settings that open a new column for every sub-menu, taking over more of the screen.
- A black button-hint bar along the bottom edge.

## The lead decided (2026-11-25)
Copied into the [design doc](../design.md) as "Agreed" lines.
- **Blur** (clarified 2026-11-25): only behind Hearth's menu and keyboard, never the whole screen. Over the home screen Qt draws it. Over apps it needs the compositor to blur; labwc can't, so it's a solid tint there until niri used ([niri before 2.0.0](../decisions/0020-niri-before-2-0.md)). The earlier "home screen only; the menu never blurs open apps" came from a misreading.
- **Menu:** two ways to show it, for now while exploring: drawn inside the home screen's window over home (so it can blur), and as an overlay with a solid tint over apps. To be tried in dev.
- **Accessibility page:** gets "Reduce transparency".
- **On-screen keyboard:** the design doc's four layouts stand. Single row = the tvOS style, with blur behind it as the menu has: a solid tint over apps on labwc, blur on niri (clarified 2026-11-25). The QWERTY grid may become an alternate layout of the alphabet grid, coming in from the side. Gamepad wheel: in scope.
- **App tile art:** theme packs can supply app art. Fetching art online: undecided. The default tile (app icon, user-settable name in the system font, automatic background colour) is the lead's idea; tile art as a whole needs scoping later.
- **Guest profile:** see the design doc's Guest profile section.

## Round 2 review (the lead, 2026-11-26)
- **Blur:** the lead likes the blurred menu panel and wants it over apps too. "Never blurs open apps" came from a misunderstanding: Claude meant blurring what's behind the panel, not the whole page. Over apps this needs compositor blur, which labwc lacks: a larger labwc question for later.
- **Home tiles:** no frosted tiles without a background colour. Asked to see frosted tiles with coloured backgrounds.
- **Grid keyboard:** alphabet and QWERTY must be the same size. Word suggestions go below the keyboard, not under the field ("I think"). The text field belongs in the keyboard panel, not on the page (Claude's reading of "the search field should be in the menu").
- **Single-row keyboard:** needs more work. Keys bottom-aligned; word suggestions above the row; the abc / ABC / 123 / #+= buttons below it.
- **Gamepad wheel:** needs dedicated time to iterate.
- **Profile picture picker:** Featured is a row that can be turned off, and shows only the pictures that come with Hearth. Open: how users add pictures from USB to each category, and add their own categories.
- **Leaving the guest profile:** centre placement. No tile highlight behind the pop-up; focus is on the buttons.
- **Failure screen:** the user should see what actually went wrong, not just a code. The code stays, for developers.
- **Volume overlay:** the top-edge version is a hard no. The right-edge version can work for development; neither is good yet.
- **Later in round 2:** the lead's sticky-note calls (keyboard Shift, single-row keyboard, profile pictures, failure screen details, Settings sub-pages, Guest PIN) went straight into the design doc as 2026-11-26 "Agreed" lines, and the principles into the [UI guide](../ui-guide.md).

## Claude's notes (proposals, not decided)
- labwc has no compositor blur: it doesn't support the standard blur protocol, ext-background-effect-v1 [C] (https://wayland.app/protocols/ext-background-effect-v1). Blur on Hearth's own screens needs nothing from it.
- Default tile colour: Flathub asks apps to declare light and dark brand colours in their AppStream metadata (`<branding>`), for use as a background behind the app icon [C] (https://docs.flathub.org/docs/for-app-authors/metainfo-guidelines/quality-guidelines). That data already comes with installed apps and the Flathub catalogue, so no new connection. When an app has none, take the icon's main colour, then darken or lighten it until the name passes WCAG AA contrast. KDE's Kirigami does that (ImageColors), but Hearth dropped Kirigami ([Theming](../decisions/0016-theming-own-tokens-no-kirigami.md)).
- Renamed tile names are stored per profile, beside each profile's app order.
- Online art sources found (for the later scoping):
  - SteamGridDB: community art (banners, logos, icons) for games and other apps; each user needs a free account and their own API key [S] (https://github.com/360900/tinkergame/wiki/SteamGridDB). An outside connection, so it would be named and switchable.
  - Web apps: a site's web app manifest can list icons and a theme colour [C] (https://developer.mozilla.org/en-US/docs/Web/Manifest). Read when the web app is added, from the site itself.
  - No general source of wide TV banners for Linux apps was found.
- A weather widget is a network connection to an outside service: named, switchable, probably off by default.
