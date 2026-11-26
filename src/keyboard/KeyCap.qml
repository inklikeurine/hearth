// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Controls
import Hearth.Theme

// One key. The keyboard never has the focus, so the highlighted key is a property
// of its own.
TvButton {
    id: control

    property bool current: false
    // A word, such as "Enter", rather than a character.
    property bool word: false

    activeFocusOnTab: false
    focusPolicy: Qt.NoFocus
    padding: Theme.tokens.sizes.outline
    z: current ? 1 : 0

    background: Rectangle {
        border.color: control.Theme.tokens.colors.outline
        border.width: control.Theme.tokens.sizes.outline
        color: control.current ? control.Theme.tokens.colors.accent : control.Theme.tokens.colors.surface
        radius: control.Theme.tokens.sizes.radius / 2

        FocusRing {
            radius: control.Theme.tokens.sizes.radius / 2 + border.width + space
            target: control
            visible: control.current
        }
    }
    contentItem: Label {
        Accessible.ignored: true
        color: control.current ? control.Theme.tokens.colors.textOnAccent : control.Theme.tokens.colors.text
        font.pixelSize: control.word ? control.Theme.tokens.sizes.textSmall : control.Theme.tokens.sizes.textBody
        horizontalAlignment: Text.AlignHCenter
        text: control.text
        verticalAlignment: Text.AlignVCenter
    }
}
