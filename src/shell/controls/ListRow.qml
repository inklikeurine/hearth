// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Theme

// One row of a list: a name, and what's on its right.
TvButton {
    id: control

    // Smaller text under the name.
    property string detail
    property string iconName
    // What goes on the right of the row.
    default property alias trailing: side.data

    Accessible.description: control.detail
    implicitHeight: Math.max(words.implicitHeight, side.implicitHeight) + topPadding + bottomPadding
    implicitWidth: 640 * Theme.tokens.screenHeight / 1080
    leftPadding: Theme.tokens.sizes.gap
    rightPadding: leftPadding
    topPadding: Theme.tokens.sizes.gap * 0.75
    bottomPadding: topPadding

    background: Rectangle {
        border.color: control.Theme.tokens.colors.outline
        border.width: control.Theme.tokens.sizes.outline
        color: control.Theme.tokens.colors.surface
        radius: control.Theme.tokens.sizes.radius

        FocusRing {
            target: control
        }
    }
    contentItem: Item {
        Icon {
            id: picture

            anchors.verticalCenter: parent.verticalCenter
            name: control.iconName
            visible: control.iconName.length > 0
        }

        Column {
            id: words

            anchors.left: picture.visible ? picture.right : parent.left
            anchors.leftMargin: picture.visible ? control.Theme.tokens.sizes.gap : 0
            anchors.right: side.left
            anchors.rightMargin: control.Theme.tokens.sizes.gap
            anchors.verticalCenter: parent.verticalCenter

            Label {
                Accessible.ignored: true
                text: control.text
                width: parent.width
            }

            Label {
                Accessible.ignored: true
                color: control.Theme.tokens.colors.textDim
                elide: Text.ElideNone
                font.pixelSize: control.Theme.tokens.sizes.textSmall
                text: control.detail
                visible: text.length > 0
                width: parent.width
                wrapMode: Text.Wrap
            }
        }

        Row {
            id: side

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: control.Theme.tokens.sizes.gap / 2
        }
    }
}
