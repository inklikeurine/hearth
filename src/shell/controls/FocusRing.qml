// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Theme

// The ring around whatever the remote is on. It sits outside its parent's edge,
// with a gap, so it stands out even when its colour is close to the control's.
Rectangle {
    id: ring

    required property Item target
    readonly property int space: border.width

    Accessible.ignored: true
    anchors.fill: parent
    anchors.margins: -(border.width + space)
    border.color: Theme.tokens.colors.focusRing
    border.width: Theme.tokens.sizes.focusRing
    color: "transparent"
    radius: Theme.tokens.sizes.radius + border.width + space
    visible: ring.target.activeFocus
}
