// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Theme

// A round button with a picture. Its text is for the screen reader, and for a
// hint under it while it's focused.
TvButton {
    id: control

    // A number on the corner, such as how many notifications there are.
    property int badge: 0
    property string iconName

    Accessible.description: control.badge > 0 ? control.badge.toString() : ""
    implicitHeight: Theme.tokens.sizes.textBody * 2
    implicitWidth: implicitHeight
    padding: Theme.tokens.sizes.textBody / 2

    background: Rectangle {
        border.color: control.Theme.tokens.colors.outline
        border.width: control.Theme.tokens.sizes.outline
        color: control.Theme.tokens.colors.surface
        radius: control.Theme.tokens.sizes.radius

        FocusRing {
            target: control
        }
    }
    contentItem: Icon {
        name: control.iconName
    }

    Rectangle {
        Accessible.ignored: true
        color: control.Theme.tokens.colors.accent
        height: control.Theme.tokens.sizes.textSmall * 1.2
        radius: height / 2
        visible: control.badge > 0
        width: Math.max(height, number.implicitWidth + height / 2)
        x: control.width - width / 2
        y: -height / 2

        Label {
            id: number

            Accessible.ignored: true
            anchors.centerIn: parent
            color: control.Theme.tokens.colors.textOnAccent
            font.pixelSize: control.Theme.tokens.sizes.textSmall
            text: control.badge > 99 ? "99+" : control.badge
        }
    }

    Label {
        Accessible.ignored: true
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.bottom
        anchors.topMargin: control.Theme.tokens.sizes.gap / 2
        elide: Text.ElideNone
        font.pixelSize: control.Theme.tokens.sizes.textSmall
        text: control.text
        visible: control.activeFocus
    }
}
