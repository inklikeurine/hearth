// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Theme

TvButton {
    id: control

    // The main thing to do on a screen.
    property bool primary: false

    implicitHeight: implicitContentHeight + topPadding + bottomPadding
    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    leftPadding: Theme.tokens.sizes.gap * 1.5
    rightPadding: leftPadding
    topPadding: Theme.tokens.sizes.gap * 0.75
    bottomPadding: topPadding

    background: Rectangle {
        border.color: control.Theme.tokens.colors.outline
        border.width: control.Theme.tokens.sizes.outline
        color: control.primary ? control.Theme.tokens.colors.accent : control.Theme.tokens.colors.surface
        radius: control.Theme.tokens.sizes.radius

        FocusRing {
            target: control
        }
    }
    contentItem: Label {
        Accessible.ignored: true
        color: control.primary ? control.Theme.tokens.colors.textOnAccent : control.Theme.tokens.colors.text
        horizontalAlignment: Text.AlignHCenter
        text: control.text
        verticalAlignment: Text.AlignVCenter
    }
}
