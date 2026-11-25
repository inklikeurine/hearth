// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtTest
import Hearth.Controls
import Hearth.Theme

Item {
    id: root

    height: 600
    width: 1200

    ThemeTokens {
        id: preview

        Component.onCompleted: loadBuiltin("high-contrast")
    }

    Row {
        spacing: 60

        Tile {
            id: tile

            text: "A long app name that can't possibly fit under its tile"
        }

        Button {
            id: button

            text: "OK"
        }

        Item {
            id: previewArea

            Theme.tokens: preview
            height: 200
            width: 300

            Button {
                id: previewButton

                text: "Preview"
            }
        }
    }

    SignalSpy {
        id: tileClicks

        signalName: "clicked"
        target: tile
    }

    TestCase {
        function init() {
            tileClicks.clear();
            tile.forceActiveFocus();
        }

        function test_accessibleNames() {
            compare(tile.Accessible.name, tile.text);
            compare(tile.Accessible.role, Accessible.Button);
            compare(button.Accessible.name, "OK");
        }

        function test_clickPressesAndTakesFocus() {
            button.forceActiveFocus();
            mouseClick(tile);
            compare(tileClicks.count, 1);
            verify(tile.activeFocus);
        }

        function test_focusNeverFollowsThePointer() {
            mouseMove(button, 2, 2);
            mouseMove(button, button.width / 2, button.height / 2);
            verify(tile.activeFocus);
            verify(!button.hovered);
        }

        function test_focusRingShowsOnlyOnTheFocusedControl() {
            const ringOf = item => item instanceof FocusRing ? item : Array.from(item.children).map(ringOf).find(ring => ring);
            verify(ringOf(tile).visible);
            verify(!ringOf(button).visible);
            verify(tile.z > button.z);
            button.forceActiveFocus();
            verify(!ringOf(tile).visible);
            verify(ringOf(button).visible);
        }

        function test_tileRingIsAroundThePictureOnly() {
            const ringOf = item => item instanceof FocusRing ? item : Array.from(item.children).map(ringOf).find(ring => ring);
            verify(ringOf(tile).height < tile.height);
        }

        function test_longNamesStayInsideTheTile() {
            for (const scale of [1.0, 1.5]) {
                tile.Theme.tokens.textScale = scale;
                verify(tile.contentItem.childrenRect.width <= tile.width);
                verify(tile.contentItem.childrenRect.height <= tile.height);
            }
            tile.Theme.tokens.textScale = 1.0;
        }

        function test_otherKeysPassThrough() {
            keyClick(Qt.Key_A);
            compare(tileClicks.count, 0);
        }

        function test_previewUsesItsOwnTheme() {
            compare(previewButton.Theme.tokens, preview);
            verify(button.Theme.tokens !== preview);
            verify(!Qt.colorEqual((previewButton.background as Rectangle).color, (button.background as Rectangle).color));
        }

        function test_selectPressesOnce_data() {
            return [
                {
                    tag: "Return",
                    key: Qt.Key_Return
                },
                {
                    tag: "Enter",
                    key: Qt.Key_Enter
                },
                {
                    tag: "Select",
                    key: Qt.Key_Select
                }
            ];
        }

        function test_selectPressesOnce(data) {
            keyClick(data.key);
            compare(tileClicks.count, 1);
        }

        function test_themeChangeReachesControls() {
            const before = (button.background as Rectangle).color.toString();
            verify(button.Theme.tokens.loadBuiltin("high-contrast"));
            verify((button.background as Rectangle).color.toString() !== before);
            compare((button.background as Rectangle).border.width, 3);
            verify(button.Theme.tokens.loadBuiltin("dark"));
            compare((button.background as Rectangle).color.toString(), before);
        }

        name: "Controls"
        when: windowShown
    }
}
