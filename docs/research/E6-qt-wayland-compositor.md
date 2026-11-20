# E6: Qt Wayland Compositor (Qt 6.11+)
 
Tags: [C] primary source read, [S] secondary, [M] memory. Source: github.com/qt/qtwayland v6.11.3 (also checked v6.12.0, dev).
 
## Answer
It covers the basic app protocols but has no layer-shell, virtual-keyboard, fractional-scale, colour management/HDR or screen-reader support. It is meant to run on eglfs. Products use it inside bigger stacks (Qt Application Manager, webOS OSE).
 
## 1. Protocols (compositor side, apps as clients)
| Protocol | Supported? | Since Qt | Link |
|---|---|---|---|
| xdg-shell | Yes [C] | 5.12 (`\since`) | [qwaylandxdgshell.cpp](https://github.com/qt/qtwayland/blob/v6.11.3/src/compositor/extensions/qwaylandxdgshell.cpp) |
| wlr-layer-shell | NOT FOUND | n/a | [CMakeLists](https://github.com/qt/qtwayland/blob/v6.11.3/src/compositor/CMakeLists.txt) |
| text-input v2 | Yes [C] | 5.7 (first tag with the file) | [qwaylandtextinputmanager.cpp](https://github.com/qt/qtwayland/blob/v6.11.3/src/compositor/extensions/qwaylandtextinputmanager.cpp) |
| text-input v3 | Yes [C] | 6.3 (first tag with the file) | [v3 file](https://github.com/qt/qtwayland/blob/v6.11.3/src/compositor/extensions/qwaylandtextinputmanagerv3.cpp) |
| input-method (zwp v1/v2) | No. Only Qt's own `qt-text-input-method-unstable-v1` [C] | n/a | [CMakeLists](https://github.com/qt/qtwayland/blob/v6.11.3/src/compositor/CMakeLists.txt) |
| virtual-keyboard | NOT FOUND | n/a | same |
| xdg-decoration | Yes [C] | 5.12 (`\since`) | [file](https://github.com/qt/qtwayland/blob/v6.11.3/src/compositor/extensions/qwaylandxdgdecorationv1.cpp) |
| viewporter | Yes [C] | 5.13 (`\since`) | [file](https://github.com/qt/qtwayland/blob/v6.11.3/src/compositor/extensions/qwaylandviewporter.cpp) |
| linux-dmabuf | Yes, as an optional plugin (build feature `wayland_dmabuf_client_buffer`) [C] | 5.13 (first tag with the plugin) | [plugin dir](https://github.com/qt/qtwayland/tree/v6.11.3/src/hardwareintegration/compositor/linux-dmabuf-unstable-v1) |
| presentation-time | Yes [C] | 6.3 (`\since`) | [file](https://github.com/qt/qtwayland/blob/v6.11.3/src/compositor/extensions/qwaylandpresentationtime.cpp) |
| fractional-scale | NOT FOUND in compositor code. Only a client test mock [C] | n/a | [test mock](https://github.com/qt/qtwayland/blob/v6.11.3/tests/auto/client/shared/fractionalscalev1.h) |
| colour-management / HDR | NOT FOUND in compositor code | n/a | n/a |
 
Conflict: the [Qt 6 docs page](https://doc.qt.io/qt-6/qtwaylandcompositor-index.html) lists "Color Management v1" and "Fractional Scale v1" in its attributions [C]. I found no compositor code for either and can't say why they're listed.
 
## 2. Running directly on the display (eglfs KMS/DRM)
- Yes in intent [C]. The [minimal-qml docs](https://github.com/qt/qtwayland/blob/v6.11.3/examples/wayland/minimal-qml/doc/src/minimal-qml.qdoc) say that as the only display server, it will probably run on eglfs. The [qtshell example](https://github.com/qt/qtwayland/blob/v6.11.3/examples/wayland/qtshell/qml/CompositorScreen.qml) gives eglfs advice.
- Display mode is chosen by the eglfs backend, not the compositor: `QT_QPA_EGLFS_KMS_CONFIG` JSON, key `mode` (`current`, `preferred`, `WxH`, `WxH@refresh`, modeline) [C, [Qt embedded Linux docs](https://doc.qt.io/qt-6/embedded-linux.html)].
- Not confirmed: dmabuf buffers on eglfs. Needs a test on real Intel hardware.
## 3. Accessibility for hosted apps
NOT FOUND. No accessibility code or docs in `src/compositor` [C]. (The only "accessib" match is an unrelated runtime-directory phrase.) Apps may still talk AT-SPI over D-Bus on their own. Untested [M].
 
## 4. Shipping products using it as main compositor
- **Qt Application Manager** (Qt's own, automotive/embedded): its compositor is [based on the QtWayland module](https://doc.qt.io/QtApplicationManager-6.11/introduction.html) [C].
- **webOS OSE, Luna Surface Manager** (LG): a [Wayland compositor written in QML on QtWayland](https://www.webosose.org/docs/guides/core-topics/graphics-input/) [C]. Its [build recipe](https://layers.openembedded.org/layerindex/recipe/78894/) depends on `qtwayland` [S]. [LuneOS](https://github.com/webOS-ports/luneos-releases/releases/) adopted it in 2024 [S].
- **Sailfish OS Lipstick**: Qt-based but on a very old QtWayland; [the community discusses replacing it](https://forum.sailfishos.org/t/wayland-compositor-experiments/26645) [S]. Not Qt 6.
- NOT FOUND: any retail TV or HTPC product on Qt 6.11.
## Hearth fit
No ground-rule conflict. Without layer-shell, Hearth's home screen must be the compositor itself.
 
