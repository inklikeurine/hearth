// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Templates as T
import Hearth.Theme

// A line of text to type. The on-screen keyboard opens when it's focused.
T.TextField {
    id: control

    // What it's for, above the box.
    property string label

    Accessible.name: control.label
    Accessible.role: Accessible.EditableText
    color: Theme.tokens.colors.text
    font.family: Theme.tokens.fontFamily
    font.pixelSize: Theme.tokens.sizes.textBody
    implicitHeight: font.pixelSize * 2.2 + topPadding
    implicitWidth: 640 * Theme.tokens.screenHeight / 1080
    leftPadding: Theme.tokens.sizes.gap
    placeholderTextColor: Theme.tokens.colors.textDim
    rightPadding: leftPadding
    selectedTextColor: Theme.tokens.colors.textOnAccent
    selectionColor: Theme.tokens.colors.accent
    topPadding: control.label.length > 0 ? Theme.tokens.sizes.textSmall * 1.6 : 0
    verticalAlignment: TextInput.AlignVCenter
    z: activeFocus ? 1 : 0

    background: Item {
        Rectangle {
            border.color: control.Theme.tokens.colors.textDim
            border.width: 2
            color: control.Theme.tokens.colors.surface
            height: parent.height - control.topPadding
            radius: control.Theme.tokens.sizes.radius
            width: parent.width
            y: control.topPadding

            FocusRing {
                target: control
            }
        }

        Label {
            Accessible.ignored: true
            color: control.Theme.tokens.colors.textDim
            font.pixelSize: control.Theme.tokens.sizes.textSmall
            text: control.label
            width: parent.width
        }
    }
}
