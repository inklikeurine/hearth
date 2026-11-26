// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick

// One of a few named values, changed with left and right. Select goes to the
// next one, and from the last back to the first.
ListRow {
    id: control

    property int current: 0
    property list<string> options: []

    signal chosen(int index)

    Accessible.description: control.options[control.current] ?? ""
    Accessible.role: Accessible.ComboBox
    Keys.onLeftPressed: {
        if (control.current > 0) {
            control.chosen(control.current - 1);
        }
    }
    Keys.onRightPressed: {
        if (control.current + 1 < control.options.length) {
            control.chosen(control.current + 1);
        }
    }

    onClicked: {
        if (control.options.length > 1) {
            control.chosen((control.current + 1) % control.options.length);
        }
    }

    Icon {
        anchors.verticalCenter: parent.verticalCenter
        name: "left"
        opacity: control.current > 0 ? 1 : 0
    }

    Label {
        Accessible.ignored: true
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: Text.AlignHCenter
        objectName: "value"
        text: control.options[control.current] ?? ""
        width: control.width * 0.3
    }

    Icon {
        anchors.verticalCenter: parent.verticalCenter
        name: "right"
        opacity: control.current + 1 < control.options.length ? 1 : 0
    }
}
