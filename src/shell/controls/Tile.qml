// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Theme

// One app on the home screen: a card with its picture, or its first letter when
// it has none, and its name underneath.
TvButton {
    id: control

    property url picture

    implicitHeight: implicitWidth * 0.625 + name.implicitHeight + Theme.tokens.sizes.gap
    implicitWidth: 320 * Theme.tokens.screenHeight / 1080
    padding: 0
    scale: activeFocus ? Theme.tokens.motion.focusScale : 1.0

    background: Item {}
    contentItem: Item {
        Rectangle {
            id: card

            border.color: control.Theme.tokens.colors.outline
            border.width: control.Theme.tokens.sizes.outline
            color: control.Theme.tokens.colors.surface
            height: parent.height - name.height - control.Theme.tokens.sizes.gap
            radius: control.Theme.tokens.sizes.radius
            width: parent.width

            FocusRing {
                target: control
            }

            Image {
                id: image

                Accessible.ignored: true
                anchors.centerIn: parent
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                height: card.height * 0.55
                source: control.picture
                sourceSize: Qt.size(width, height)
                visible: status === Image.Ready
                width: height
            }

            Label {
                Accessible.ignored: true
                anchors.centerIn: parent
                font.pixelSize: card.height * 0.5
                text: control.text.charAt(0)
                visible: !image.visible
            }
        }

        Label {
            id: name

            Accessible.ignored: true
            anchors.bottom: parent.bottom
            color: control.activeFocus ? control.Theme.tokens.colors.text : control.Theme.tokens.colors.textDim
            font.pixelSize: control.Theme.tokens.sizes.textSmall
            horizontalAlignment: Text.AlignHCenter
            text: control.text
            width: parent.width
        }
    }
    Behavior on scale {
        ScaleAnimator {
            duration: control.Theme.tokens.motion.duration
            easing.bezierCurve: control.Theme.tokens.motion.easing
            easing.type: Easing.BezierSpline
        }
    }
}
