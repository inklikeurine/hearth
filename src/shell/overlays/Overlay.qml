// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Overlays

// A window over the whole screen, above every app. Use "shown", not "visible": a
// window's place among the layers can't change once it has been on screen.
Window {
    id: overlay

    // For labwc; short, with no spaces.
    required property string name
    property bool ready: false
    property bool shown: false
    // Takes every key while it's shown, so the app behind gets none.
    property bool takesKeyboard: true

    color: "transparent"
    flags: Qt.FramelessWindowHint
    height: 1080
    title: overlay.name
    visible: overlay.ready && overlay.shown
    width: 1920

    Component.onCompleted: {
        LayerSurface.setUp(overlay, overlay.name, overlay.takesKeyboard);
        overlay.ready = true;
    }
}
