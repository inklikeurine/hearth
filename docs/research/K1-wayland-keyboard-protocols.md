# K1 — Wayland keyboard protocols
 
Tags: [C] read in the protocol definition; [S] secondary source or mirror; [M] from memory. Nothing here is decided.
 
## 1. What each does
 
- **input-method v1**: a program (the "input method", e.g. an on-screen keyboard) receives the focused text box's state and sends back composed text. Also has a panel for keyboard windows. In wayland-protocols. [C] https://wayland.app/protocols/input-method-unstable-v1
- **input-method v2**: same job, redone. One object per seat, active only while a text box is focused; can also grab the real keyboard. In wlroots, not wayland-protocols. [C] https://wayland.app/protocols/input-method-unstable-v2
- **text-input v1**: the app side of v1. The app activates a text input and asks for the keyboard panel to show or hide. [S] (mirror; official page unreadable) https://coral.googlesource.com/wayland-protocols-imx/+/refs/heads/alpha/unstable/text-input/text-input-unstable-v1.xml
- **text-input v2**: app side with an `enable` request and batched `update_state`. Not on wayland.app; Qt ships a copy. [S] https://code.qt.io/cgit/qt/qtwayland.git/plain/src/3rdparty/protocol/text-input-unstable-v2.xml. Whether wayland-protocols ever merged it: NOT FOUND (a mirror of that directory lists only v1 and v3 [S] https://coral.googlesource.com/wayland-protocols-imx/+/refs/heads/master/unstable/text-input/).
- **text-input v3**: app side, current. Lets compositors act as input methods and send text to apps. [C] https://wayland.app/protocols/text-input-unstable-v3
- **virtual-keyboard v1**: a program acts as a physical keyboard (keymap, key, modifiers); compositors should refuse untrusted clients. In wlroots. [C] https://wayland.app/protocols/virtual-keyboard-unstable-v1
## 2. text-input v3 (link above)
 
- **Focus:** compositor sends `enter(surface)`. The app answers `enable` for the focused box (again each time focus moves between boxes, even in one surface), then `commit`. `disable` when no box has focus. [C]
- **Password:** `set_content_type(hint, purpose)`. Purpose `password` (8) or `pin` (9); the XML says to combine with hint `sensitive_data` (0x80, typed text should not be stored). Hint `hidden_text` (0x40) means characters should be hidden. Defaults: none / normal. Applied on `commit`. [C]
## 3. Path of one typed letter
 
1. Compositor → app: text-input v3 `enter`. [C]
2. App → compositor: `enable`, content type, `commit`. [C]
3. Compositor → keyboard: input-method v2 `activate`, `surrounding_text`, `content_type`, `done`. [C] that these events exist; [M] that the compositor sends them in response to step 2.
4. User taps "a". Keyboard → compositor: `commit_string("a")`, then `commit(serial)`; serial = number of `done` events so far. [C]
5. Compositor → app: text-input v3 `commit_string("a")`, then `done`. App inserts the text. [C] events; [M] relay.
Keys with no text (Enter, arrows) don't fit this path. virtual-keyboard says it can be used alongside the input method protocol. [C]
 
## Couldn't confirm
 
- gitlab.freedesktop.org blocked my fetcher; I read wayland.app pages instead, through a summarising tool, not raw XML.
- wayland.app text-input v1 and v2 pages failed to load.
- The compositor's relay in steps 3 and 5 is not spelled out in either protocol. Needs testing on a real compositor.
 
