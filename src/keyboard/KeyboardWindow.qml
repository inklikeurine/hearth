// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import org.kde.ki18n
import Hearth.Keyboard
import Hearth.Theme

// The strip along the bottom of the screen. The program makes it one
// (WaylandLink::makeStrip); here it's a window.
Window {
    id: window

    required property Keyboard keyboard

    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowDoesNotAcceptFocus
    height: view.implicitHeight
    title: KI18n.i18n("On-screen keyboard")
    visible: window.keyboard.shown
    width: Screen.width

    // The profile's theme, as the shell uses it.
    function useTheme() {
        if (!window.Theme.tokens.loadBuiltin(window.keyboard.settings.theme)) {
            // A theme this version doesn't have.
            window.Theme.tokens.loadBuiltin("dark");
        }
    }

    Component.onCompleted: window.useTheme()
    onVisibleChanged: {
        if (visible) {
            appear.restart();
        }
    }

    Connections {
        function onLookChanged() {
            window.useTheme();
        }

        target: window.keyboard.settings
    }

    Binding {
        property: "screenHeight"
        target: window.Theme.tokens
        value: window.Screen.height
    }

    Binding {
        property: "textScale"
        target: window.Theme.tokens
        value: window.keyboard.settings.textScale
    }

    Binding {
        property: "reducedMotion"
        target: window.Theme.tokens
        value: window.keyboard.settings.reducedMotion
    }

    KeyboardView {
        id: view

        height: parent.height
        keys: window.keyboard.keys
        note: window.keyboard.sensitive ? KI18n.i18n("Password: what you type stays hidden and isn't remembered.") : ""
        width: parent.width
    }

    // Slides in, or fades in with reduced motion.
    ParallelAnimation {
        id: appear

        NumberAnimation {
            duration: window.Theme.tokens.motion.duration
            easing.bezierCurve: window.Theme.tokens.motion.easing
            easing.type: Easing.BezierSpline
            from: window.Theme.tokens.reducedMotion ? 0 : view.height
            property: "y"
            target: view
            to: 0
        }

        NumberAnimation {
            duration: window.Theme.tokens.motion.duration
            from: window.Theme.tokens.reducedMotion ? 0 : 1
            property: "opacity"
            target: view
            to: 1
        }
    }
}
