// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Templates as T
import Hearth.Theme

// What every pressable control shares. The remote's Select presses it, once
// however long it's held. The pointer can click it, but moving the pointer over
// it changes nothing: focus never follows the pointer.
T.AbstractButton {
    id: control

    Accessible.name: control.text
    Accessible.role: Accessible.Button
    activeFocusOnTab: true
    focusPolicy: Qt.StrongFocus
    font.family: Theme.tokens.fontFamily
    font.pixelSize: Theme.tokens.sizes.textBody
    hoverEnabled: false
    // Above its neighbours, so none of them draws over the ring.
    z: activeFocus ? 1 : 0

    Keys.onPressed: event => {
        const select = event.key === Qt.Key_Return || event.key === Qt.Key_Enter || event.key === Qt.Key_Select;
        if (!select) {
            return;
        }
        event.accepted = true;
        if (!event.isAutoRepeat) {
            control.click();
        }
    }
}
