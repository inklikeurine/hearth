// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Controls
import Hearth.Theme

Window {
    id: window

    color: Theme.tokens.colors.background
    height: 1080
    visible: true
    width: 1920

    Column {
        anchors.fill: parent
        anchors.leftMargin: Theme.tokens.sizes.safeSide
        anchors.rightMargin: Theme.tokens.sizes.safeSide
        anchors.topMargin: Theme.tokens.sizes.safeVertical
        spacing: Theme.tokens.sizes.gap * 2

        Label {
            font.pixelSize: Theme.tokens.sizes.textTitle
            text: "Controls"
        }

        TileGrid {
            id: tiles

            KeyNavigation.down: ok
            KeyNavigation.priority: KeyNavigation.AfterItem
            focus: true
            // Tiles have room around them for the ring; this lines them up with the text.
            width: parent.width + 2 * inset
            x: -inset

            model: ListModel {
                ListElement {
                    name: "Films"
                    picture: ""
                }
                ListElement {
                    name: "Music"
                    picture: ""
                }
                ListElement {
                    name: "A name much too long to fit under its tile"
                    picture: ""
                }
                ListElement {
                    name: "Games"
                    picture: ""
                }
                ListElement {
                    name: "Settings"
                    picture: ""
                }
                ListElement {
                    name: "Photos"
                    picture: ""
                }
            }
        }

        Row {
            spacing: Theme.tokens.sizes.gap * 2

            Button {
                id: ok

                KeyNavigation.right: cancel
                KeyNavigation.up: tiles
                primary: true
                text: "Apply"
            }

            Button {
                id: cancel

                KeyNavigation.left: ok
                KeyNavigation.up: tiles
                text: "Cancel"
            }
        }

        Label {
            color: Theme.tokens.colors.textDim
            font.pixelSize: Theme.tokens.sizes.textSmall
            text: "Arrows move, Enter presses."
        }
    }
}
