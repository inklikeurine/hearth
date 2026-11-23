# Shell spike: results so far

*Claude, 2026-11-22. The spike code is on the local-only `spikes` branch (`spikes/shell/`, its README has the details). Everything below ran headless: labwc with no screen in the dev box, keys pressed with wtype, windows checked with wlrctl, pictures saved by the shell. The latest run passed 69 of 69 checks, twice in a row. The lead has run an earlier build in a window; nobody has watched the keyboard on a screen yet.*

*Tags: [T] = seen in the spike's own test runs; [C] = read in source or installed files; [M] = Claude's memory, unchecked.*

## Spike checks (design doc, section 5.4)

| Check | Result |
|---|---|
| 1. Bare home screen | Works: a full-screen window with the user's background, the status bar, tiles in rows of five with the next row peeking in, arrow keys moving focus and scrolling rows |
| 2. Firefox full screen | Works: kiosk mode, full screen, on top of the home window |
| 3. Menu over Firefox | Works: a layer-shell overlay over the full-screen app, taking the keys while open |
| 4. Typing | Works, with a small test keyboard in the shell instead of Stevia (see below). The remote's arrows and Select typed "hi" into a Firefox text box; Backspace and Enter reached the page; Firefox kept focus and stayed full screen [T] |
| 5. Going home | Works: Home (a labwc key binding) minimizes the app and brings the home window forward; the app's tile brings it back un-minimized, without restarting it |
| 6. Back key | Works. labwc takes the remote's Back; the shell sends a `back` action over native messaging to a TV-navigation extension, which closes the page's open menu, else goes back a page, else leaves the app (the home screen comes back). Firefox stayed full screen; the pages never saw Back or Escape. Slowest round trip, shell → extension → page → shell: 7–8 ms (target: 50 ms) |
| 12. Forcing full screen | Works with a labwc window rule using `ToggleFullscreen`: foot, which opens as a window, goes full screen, and Firefox in kiosk mode stays full screen. `Maximize` didn't take effect at all |

Checks 7 (phone typing) and 8–11 (real screen, RAM, overlay over video, TV colour range) are still to do. The lead's order: software first, real hardware after; phone typing later.

## The lead's run in a window (2026-11-22)

- The UI is serviceable and clear for a spike (the lead).
- Back closes Dropout, Twitch and YouTube, but does nothing in Firefox opened from its own tile. Expected from the code: only web apps got the TV-navigation extension; for any other app the shell answered `no-adapter` and did nothing.
- Fixed since: Firefox's own tile now starts like a web app, with its own profile and the extension but no kiosk mode. Back on its start page leaves it [T]. Firefox's own pages (the new-tab page, `about:` pages) don't take extension scripts, so the spike's rule there is "Back leaves".

## Check 4, typing: first try

- Stevia can't run on labwc. Both Fedora's 0.57.0 and upstream main (2026-11-21) require `zphoc_device_state_v1`, phoc's private protocol, before they create the keyboard (`pos_wayland_has_wl_protcols()`, `src/pos-wayland.c`) [C]. Under headless labwc it bound everything else (input-method v2, virtual-keyboard, layer-shell) and logged "Wayland not yet ready, skipping input surface creation".
- The dev box passes the host desktop's IBus settings (`GTK_IM_MODULE=ibus`, `QT_IM_MODULE`, `XMODIFIERS`) to apps, so Firefox sends text boxes to IBus and never asks labwc for a keyboard. The spike's session (`session.sh`) drops them.
- The protocol files a home-made test keyboard needs (`input-method-unstable-v2.xml`, `virtual-keyboard-unstable-v1.xml`) aren't in the box's approved packages. Fedora has them only inside a Rust crate package (`rust-wayland-protocols-misc-devel`, which pulls in cargo and Rust); Stevia's repo carries copies (MIT-style licence).

## Check 4, typing: the test keyboard

How it works: the shell has a keyboard of its own (`src/inputmethod.cpp`, `qml/KeyboardWindow.qml`), borrowing Stevia's two protocol files (pinned, `protocol/README.md`). Firefox tells labwc a text box has focus (text-input v3); labwc tells the keyboard (input-method v2); the keyboard appears as a layer-shell strip along the bottom and grabs the keys, so arrows and Select work the keyboard while Firefox keeps its text box focused. Text goes back through input-method v2; Backspace and Enter through virtual-keyboard v1.

Results [T, headless test, two runs]:

| Question | Answer |
|---|---|
| Does a remote drive it? | Yes: arrows and Select through the keyboard grab; the page never saw them |
| Does the remote's Back close it? | Yes: labwc's key binding gets Back before the grab does, so the shell closes the keyboard first; the page stays put, Firefox stays full screen |
| Esc inside the keyboard | Reaches the keyboard through the grab; it closes |
| Home with the keyboard open | Goes home; the keyboard goes too (Firefox's box loses focus) |
| Back to the app, box still focused | The keyboard comes back by itself |
| Menu over the keyboard | The menu takes the keys and the keyboard goes; closing the menu brings it back |
| A real keyboard's keys while it's shown | Swallowed by the grab ("1" and "2" never reached the page) |

What each kind of web field asks for (Firefox 160, text-input v3's content purpose and hints) [T]:

| Field | Keyboard shown? | Purpose | Hints |
|---|---|---|---|
| `<input>` | yes | normal | none |
| `type=password` | yes | password | `0xc0`: hidden text and sensitive data [M for the bit meanings] |
| `type=email` | yes | email | none |
| `type=number` | yes | number | none |
| `type=tel` | yes | phone | none |
| `type=url` | yes | url | none |
| `type=search` | yes | normal | none |
| `inputmode="none"` | **yes** | normal | none |
| `inputmode="numeric"` | yes | digits | none |
| `<textarea>` | yes | normal | none (no "multiline" hint) |

- **`inputmode="none"` doesn't reach the keyboard.** Firefox switches text input on for it like any box and says nothing about the mark. This is how Leanback for Dropout stops system keyboards opening over its own one (section 6.4), so through text-input v3 alone Hearth's keyboard can't honour it.
- **Password text:** after one letter typed into the password box, the keyboard had been sent 0 characters of it; the plain box, after "hi", 1. That suggests Firefox doesn't send password text to the keyboard, but updates also seem to lag one keystroke, so it isn't proven.
- **Qt switches text input on with no text box.** Qt 6.11's Wayland client sends text-input `enable()` whenever one of its windows gets the keyboard, even though nothing focused accepts text (`ImEnabled` was false on every focus object) [T, Wayland message log]. labwc passes it on, so the keyboard popped up over Hearth's own home screen and ate its keys. Firefox's real boxes always describe themselves (their text, their kind); Qt's stray `enable()` describes nothing. The test keyboard now appears only for a described box (Claude's rule, under test). Every Qt app (Moonlight is one) would do the same, so the real keyboard needs this rule or an equivalent. Qt also reads `QT_WAYLAND_DISABLED_INTERFACES` [C, strings in Fedora's library], which could switch its text input off; untried.
- **Key names:** a keyboard grab sends key codes plus the keymap of whichever keyboard pressed them. wtype makes a small keymap per run, with key names as numbers (`0xff53` is Right). The spike reads names from the keymap's text with a small parser, because linking libxkbcommon isn't an approved dependency. The real keyboard should use libxkbcommon, which Qt already pulls in.
- **The keyboard doesn't repeat a held arrow:** the grab passes key presses and the repeat rate, and repeating is the keyboard's job. The test keyboard doesn't do it yet.
- **Stevia** still starts in the session and never shows; it caused no conflict.

## Found on the way

- **The home screen as a desktop** needs the window-switching protocol (wlr-foreign-toplevel-management), from Fedora's `wlr-protocols-devel`. With it the shell knows which app is in front, minimizes it, brings it back and closes it.
- **Fedora 45 packaging gap:** `qt6-qtwayland-devel` lacks a file (`qt6waylandclient_metatypes.json`) that Qt's build tools ask for when a QML program links the Wayland client library directly. Linking it through a small library of its own avoids it.
- **Firefox requires Mozilla's signature** for extensions it installs for good (Fedora's build: `MOZ_REQUIRE_SIGNING: true`, release channel). The spike installs the extension as a temporary add-on through Firefox's debugging channel, opened on a socket file in the user's private runtime folder. That's for testing only: anything running as the user can drive that Firefox while the channel is open.
- **Firefox keeps extensions out of `file://` pages**, so local test pages have to be served over HTTP. Real sites (`https://`) are fine.
- **labwc** runs its `-S` session command without a shell, so `VAR=value command` fails; `env VAR=value command` works.
- **The dev box** leaks the host desktop's settings into apps (`DISPLAY`, `SESSION_MANAGER`, `GTK_MODULES`, `XDG_DATA_DIRS`, and IBus's `GTK_IM_MODULE`, `QT_IM_MODULE`, `XMODIFIERS`); the spike strips them. The box has its own home folder (`~/.local/share/distrobox-homes/hearth-f45`), so its Firefox never touches the lead's own profile, but it shares the host desktop's session bus [T].
- **Fedora's Firefox** has system-wide folders for native messaging manifests and extensions: `/usr/lib64/mozilla/native-messaging-hosts` and `/usr/lib64/mozilla/extensions` (and `/usr/lib/mozilla/…`) [C, in the box]. Whether Firefox reads the manifest from there wasn't tested: the test needs a file added to and removed from the box's `/usr`, and Claude Code's safety check stopped the removal. Left for the lead.

## What it means for the plan (Claude's reading, not decided)

- The Back design in section 6.1 works as planned, and fast. What the product needs on top: the extension signed by Mozilla (unlisted, as Leanback for Dropout is), and the native messaging host's manifest installed system-wide in the image rather than in each user's home.
- The spike catches only the remote's Back key, through labwc. The real input service grabs remote devices so keyboards keep typing (section 6.2); a keyboard's Esc still reaches Firefox in the spike.
- Back in native apps needs the app adapters from section 6.1; the spike answers "no adapter". Later (the lead).
- Typing works on labwc with a keyboard of our own, through standard protocols. The real keyboard needs, on top of the test one:
  - a way to tell real keyboards from remotes, so a real keyboard's typing isn't swallowed. Phosh does this with phoc's private device-state protocol, which is why Stevia won't run without it; labwc has nothing like it, so Hearth's input service (section 6.2) would have to say;
  - the "describes a text box" rule against Qt's stray activations, or a better one;
  - `inputmode="none"` passed on some other way, such as `hearth-firefox-bridge` telling the shell;
  - key repeat, and libxkbcommon for key names.

## Not confirmed
- All the keyboard results are headless, with wtype standing in for the remote. Not yet seen in a window with the air mouse.
- Whether the box's Firefox reads a system-wide native messaging manifest.
- The meaning of the password box's hint bits (`0xc0`): from memory of text-input v3; its definition file isn't in the box.

- Anything on a real screen or real hardware, with the real air mouse.
- Real sites: what counts as a site's "menu" needs rules per site, as Leanback has for Dropout.
