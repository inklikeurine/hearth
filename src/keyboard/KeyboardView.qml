// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import org.kde.ki18n
import Hearth.Controls
import Hearth.Keyboard
import Hearth.Theme

// The keys, in a strip as wide as the screen.
Item {
    id: view

    readonly property real cellHeight: Math.round(Theme.tokens.sizes.textBody * 1.5 + Theme.tokens.sizes.gap / 2) + spacing
    // Other keys come at once: only the same row scrolls.
    property bool changingKeys: false
    readonly property real cellWidth: (width - 2 * Theme.tokens.sizes.safeSide + spacing) / keys.visibleWidth
    // The keys' own width. A row of keys can be wider than the screen.
    readonly property real keysWidth: keys.size.width * cellWidth - spacing
    required property KeyboardModel keys
    // A line of text over the keys, or nothing.
    property string note: ""
    // Room for the ring around the highlighted key.
    readonly property real spacing: Math.max(Theme.tokens.sizes.gap / 2, 2 * Theme.tokens.sizes.focusRing)

    function nameOf(does: int, typed: string): string {
        switch (does) {
        case KeyAction.Space:
            return KI18n.i18nc("@action:button a key on the on-screen keyboard", "Space");
        case KeyAction.Backspace:
            return KI18n.i18nc("@action:button a key: delete the letter before the cursor", "Delete");
        case KeyAction.Enter:
            return KI18n.i18nc("@action:button a key on the on-screen keyboard", "Enter");
        case KeyAction.Shift:
            return view.keys.shift === KeyboardModel.Locked ? KI18n.i18nc("@action:button a key: Caps Lock is on", "Caps") : KI18n.i18nc("@action:button a key on the on-screen keyboard", "Shift");
        case KeyAction.Symbols:
            return KI18n.i18nc("@action:button a key: show digits and symbols", "?123");
        case KeyAction.Letters:
            return KI18n.i18nc("@action:button a key: show the letters", "ABC");
        case KeyAction.Close:
            return KI18n.i18nc("@action:button a key: close the on-screen keyboard", "Close");
        default:
            return typed;
        }
    }

    implicitHeight: noteArea.height + Theme.tokens.sizes.gap + keys.size.height * cellHeight - spacing + Theme.tokens.sizes.safeVertical

    Connections {
        function onModelAboutToBeReset() {
            view.changingKeys = true;
            Qt.callLater(() => view.changingKeys = false);
        }

        target: view.keys
    }

    Rectangle {
        anchors.fill: parent
        color: view.Theme.tokens.colors.background

        Rectangle {
            color: view.Theme.tokens.colors.outline
            height: view.Theme.tokens.sizes.outline
            width: parent.width
        }
    }

    Item {
        id: noteArea

        anchors.left: parent.left
        anchors.leftMargin: Theme.tokens.sizes.safeSide
        anchors.right: parent.right
        anchors.rightMargin: Theme.tokens.sizes.safeSide
        height: visible ? noteLabel.implicitHeight + Theme.tokens.sizes.gap : 0
        visible: view.note !== ""

        Label {
            id: noteLabel

            anchors.bottom: parent.bottom
            color: Theme.tokens.colors.textDim
            font.pixelSize: Theme.tokens.sizes.textSmall
            text: view.note
            width: parent.width
        }
    }

    Item {
        id: allKeys

        height: view.keys.size.height * view.cellHeight - view.spacing
        width: view.keysWidth
        x: {
            if (width <= view.width - 2 * Theme.tokens.sizes.safeSide) {
                return (view.width - width) / 2;
            }
            // Keeps the highlighted key in the middle, as far as the row goes.
            const left = view.width - Theme.tokens.sizes.safeSide - width;
            const centred = view.width / 2 - view.keys.currentX * view.cellWidth;
            return Math.min(Math.max(centred, left), Theme.tokens.sizes.safeSide);
        }
        y: noteArea.height + Theme.tokens.sizes.gap

        Behavior on x {
            enabled: !view.Theme.tokens.reducedMotion && !view.changingKeys

            NumberAnimation {
                duration: view.Theme.tokens.motion.duration
                easing.bezierCurve: view.Theme.tokens.motion.easing
                easing.type: Easing.BezierSpline
            }
        }

        Repeater {
            model: view.keys

            KeyCap {
                id: key

                required property int does
                required property int index
                required property rect place
                required property string typed

                Accessible.checkable: does === KeyAction.Shift
                Accessible.checked: does === KeyAction.Shift && view.keys.shift !== KeyboardModel.Off
                current: index === view.keys.currentIndex
                height: place.height * view.cellHeight - view.spacing
                text: view.nameOf(does, typed)
                width: place.width * view.cellWidth - view.spacing
                word: does !== KeyAction.Type
                x: place.x * view.cellWidth
                y: place.y * view.cellHeight

                onClicked: view.keys.pressKey(index)
            }
        }
    }
}
