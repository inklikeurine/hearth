# K2 — Which text-input versions apps speak on Wayland
 
Tags: [C] read in the project's own source; [M] memory. Source read on 2026-11-20 (main/dev branches).
 
| App toolkit | text-input v1 / v2 / v3 | Since version | Source link |
|---|---|---|---|
| Firefox (GTK) | No / No / only via GTK. Firefox binds no text-input protocol itself; it uses GTK's input-method object. [C] | NOT FOUND (depends on the GTK it runs on) | [protocol list](https://github.com/mozilla-firefox/firefox/tree/main/widget/gtk/wayland), [IMContextWrapper.cpp](https://github.com/mozilla-firefox/firefox/blob/main/widget/gtk/IMContextWrapper.cpp) |
| Qt 6 | Yes / Yes / Yes. [C] | v2: Qt 5.7.0. v1: 6.4.0. v3: 6.7.0. [C] | [qwaylanddisplay.cpp](https://code.qt.io/cgit/qt/qtbase.git/tree/src/plugins/platforms/wayland/qwaylanddisplay.cpp), [tag 6.7.0](https://code.qt.io/cgit/qt/qtwayland.git/tree/src/client?h=v6.7.0) |
| SDL3 | No / No / Yes. [C] | 3.2.0 (first 3.x release with it). Added 2021-07-29; in SDL2 since 2.0.16. [C] | [SDL_waylandevents.c](https://github.com/libsdl-org/SDL/blob/main/src/video/wayland/SDL_waylandevents.c), [commit](https://github.com/libsdl-org/SDL/commit/74162b740) |
| GTK4 | No / No / Yes. [C] | 4.0.0. GTK 3.24.0 also has v3. [C] | [gtkimcontextwayland.c](https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gtk/gtkimcontextwayland.c), [tag 4.0.0](https://gitlab.gnome.org/GNOME/gtk/-/tree/4.0.0/gtk) |
 
## Notes
 
- **Qt order of preference:** its own `qt_text_input_method_v1`, then v2, then v3, then v1. The code comment says v2 wins over v3 because v3 is less featureful and KWin supports v2. By my reading it uses the best one the compositor offers. `QT_WAYLAND_TEXT_INPUT_PROTOCOL` overrides. [C]
- **v2** isn't in wayland-protocols (see K1); Qt ships its own copy. [C]
- **Recommendation (not a decision):** v3 is the one protocol all four speak, so the Hearth compositor must offer v3. Offering v2 only matters for Qt apps.
## Couldn't confirm
 
- Firefox: I searched only `widget/gtk`, not the whole tree. searchfox.org blocked my fetcher.
- GTK, SDL and Qt were read through GitHub mirrors of the official repos, not the official hosts.
- Which SDL version Moonlight ships: not checked.
- Nothing was run on a real compositor.
 
