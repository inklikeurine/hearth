// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import org.kde.ki18n
import Hearth.Theme

// A setting that's on or off. It says which in words too: colour is never the
// only sign.
ListRow {
    id: control

    property bool on: false

    signal switched(bool on)

    Accessible.checkable: true
    Accessible.checked: control.on
    Accessible.role: Accessible.CheckBox

    onClicked: control.switched(!control.on)

    Label {
        Accessible.ignored: true
        anchors.verticalCenter: parent.verticalCenter
        color: control.Theme.tokens.colors.textDim
        text: control.on ? KI18n.i18n("On") : KI18n.i18n("Off")
    }

    Rectangle {
        Accessible.ignored: true
        anchors.verticalCenter: parent.verticalCenter
        border.color: control.on ? control.Theme.tokens.colors.accent : control.Theme.tokens.colors.textDim
        border.width: 3
        color: control.on ? control.Theme.tokens.colors.accent : "transparent"
        height: control.Theme.tokens.sizes.textBody
        radius: height / 2
        width: height * 1.8

        Rectangle {
            color: control.on ? control.Theme.tokens.colors.textOnAccent : control.Theme.tokens.colors.textDim
            height: parent.height - 12
            radius: height / 2
            width: height
            x: control.on ? parent.width - width - 6 : 6
            y: 6

            Behavior on x {
                enabled: !control.Theme.tokens.reducedMotion

                NumberAnimation {
                    duration: control.Theme.tokens.motion.duration
                }
            }
        }
    }
}
