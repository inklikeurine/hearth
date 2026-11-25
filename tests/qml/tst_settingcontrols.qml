// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import QtTest
import Hearth.Controls

Item {
    id: root

    height: 1080
    width: 1920

    FocusColumn {
        id: column

        height: 420
        width: 1000

        Label {
            text: "A heading"
            width: parent.width
        }

        ListRow {
            id: row

            detail: "Some detail"
            text: "Row"
            width: parent.width
        }

        Toggle {
            id: toggle

            text: "Toggle"
            width: parent.width

            onSwitched: on => toggle.on = on
        }

        ListRow {
            id: hidden

            text: "Hidden"
            visible: false
            width: parent.width
        }

        Slider {
            id: slider

            from: 100
            step: 10
            text: "Slider"
            to: 150
            value: 100
            valueText: value + "%"
            width: parent.width

            onMoved: value => slider.value = value
        }

        Choice {
            id: choice

            options: ["One", "Two", "Three"]
            text: "Choice"
            width: parent.width

            onChosen: index => choice.current = index
        }

        TextBox {
            id: box

            label: "Name"
            width: parent.width
        }
    }

    IconButton {
        id: iconButton

        badge: 120
        iconName: "bell"
        text: "Notifications"
        x: 1200
        y: 200
    }

    TestCase {
        function init() {
            toggle.on = false;
            slider.value = 100;
            choice.current = 0;
            box.clear();
            column.start();
        }

        function test_upAndDownVisitEveryControl() {
            const seen = [row, toggle, slider, choice, box];
            for (const control of seen) {
                verify(control.activeFocus, control.toString());
                keyClick(Qt.Key_Down);
            }
            verify(box.activeFocus);
            for (const control of seen.reverse()) {
                verify(control.activeFocus, control.toString());
                keyClick(Qt.Key_Up);
            }
            verify(row.activeFocus);
        }

        function inView(control: Item): bool {
            const top = control.mapToItem(column, 0, 0).y;
            return top >= 0 && top + control.height <= column.height;
        }

        function test_theFocusedControlIsInView() {
            for (let i = 0; i < 4; ++i) {
                keyClick(Qt.Key_Down);
            }
            verify(box.activeFocus);
            tryVerify(() => inView(box), 1000, "the last control is out of view");
            for (let i = 0; i < 4; ++i) {
                keyClick(Qt.Key_Up);
            }
            tryVerify(() => inView(row), 1000, "the first control is out of view");
        }

        function test_comesBackToWhereItWas() {
            keyClick(Qt.Key_Down);
            verify(toggle.activeFocus);
            iconButton.forceActiveFocus();
            column.forceActiveFocus();
            verify(toggle.activeFocus);
        }

        function test_toggle() {
            toggle.forceActiveFocus();
            compare(toggle.Accessible.checked, false);
            keyClick(Qt.Key_Return);
            compare(toggle.on, true);
            compare(toggle.Accessible.checked, true);
            compare(toggle.Accessible.role, Accessible.CheckBox);
            keyClick(Qt.Key_Return);
            compare(toggle.on, false);
        }

        function test_sliderStopsAtItsEnds() {
            slider.forceActiveFocus();
            keyClick(Qt.Key_Left);
            compare(slider.value, 100);
            keyClick(Qt.Key_Right);
            compare(slider.value, 110);
            for (let i = 0; i < 10; ++i) {
                keyClick(Qt.Key_Right);
            }
            compare(slider.value, 150);
            compare(slider.Accessible.description, "150%");
            compare(slider.Accessible.role, Accessible.Slider);
        }

        function test_choice() {
            choice.forceActiveFocus();
            keyClick(Qt.Key_Left);
            compare(choice.current, 0);
            keyClick(Qt.Key_Right);
            keyClick(Qt.Key_Right);
            keyClick(Qt.Key_Right);
            compare(choice.current, 2);
            compare(choice.Accessible.description, "Three");
            keyClick(Qt.Key_Return);
            compare(choice.current, 0);
        }

        function test_textBox() {
            box.forceActiveFocus();
            keyClick(Qt.Key_H);
            keyClick(Qt.Key_I);
            compare(box.text, "hi");
            compare(box.Accessible.name, "Name");
        }

        function test_iconButton() {
            compare(iconButton.Accessible.name, "Notifications");
            iconButton.forceActiveFocus();
            mouseClick(iconButton);
            verify(iconButton.activeFocus);
        }

        name: "SettingControls"
        when: windowShown
    }
}
