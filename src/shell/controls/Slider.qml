// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Theme

// A number changed with left and right.
ListRow {
    id: control

    property int from: 0
    property int step: 1
    property int to: 100
    property int value: 0
    // How the value reads, such as "120%".
    property string valueText: control.value.toString()

    signal moved(int value)

    function change(steps: int) {
        const next = Math.min(control.to, Math.max(control.from, control.value + steps * control.step));
        if (next !== control.value) {
            control.moved(next);
        }
    }

    Accessible.description: control.valueText
    Accessible.role: Accessible.Slider
    Keys.onLeftPressed: control.change(-1)
    Keys.onRightPressed: control.change(1)

    Accessible.onDecreaseAction: control.change(-1)
    Accessible.onIncreaseAction: control.change(1)

    Label {
        Accessible.ignored: true
        anchors.verticalCenter: parent.verticalCenter
        color: control.Theme.tokens.colors.textDim
        horizontalAlignment: Text.AlignRight
        text: control.valueText
        width: control.Theme.tokens.sizes.textBody * 4
    }

    Item {
        Accessible.ignored: true
        anchors.verticalCenter: parent.verticalCenter
        height: control.Theme.tokens.sizes.textBody
        width: control.width * 0.35

        Rectangle {
            id: line

            anchors.verticalCenter: parent.verticalCenter
            color: control.Theme.tokens.colors.textDim
            height: 6
            radius: 3
            width: parent.width
        }

        Rectangle {
            id: filled

            anchors.verticalCenter: parent.verticalCenter
            color: control.Theme.tokens.colors.accent
            height: 10
            radius: 5
            width: control.to > control.from ? line.width * (control.value - control.from) / (control.to - control.from) : 0
        }

        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            color: control.Theme.tokens.colors.text
            height: control.Theme.tokens.sizes.textBody * 0.8
            radius: height / 2
            width: height
            x: filled.width - width / 2
        }

        MouseArea {
            anchors.fill: parent

            onClicked: mouse => {
                control.forceActiveFocus();
                const wanted = control.from + (control.to - control.from) * mouse.x / width;
                control.change(Math.round((wanted - control.value) / control.step));
            }
        }
    }
}
