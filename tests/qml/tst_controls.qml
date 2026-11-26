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

    ThemeTokens {
        id: glassTheme
    }

    // A panel over a picture, with Qt blurring it, and one with nothing to blur.
    Item {
        Theme.tokens: glassTheme
        height: 200
        width: 400
        y: 300

        // Red, with a blue stripe down the middle of the first panel.
        Rectangle {
            id: scenery

            color: "red"
            height: 200
            width: 400

            Rectangle {
                color: "blue"
                height: 200
                width: 20
                x: 190
            }
        }

        PanelBackground {
            id: overScenery

            height: 200
            source: scenery
            width: 200
            x: 100
        }

        PanelBackground {
            id: overApps

            height: 200
            width: 100
            x: 300
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

    TestCase {
        name: "PanelBackground"
        when: windowShown

        function cleanup() {
            glassTheme.reduceTransparency = false;
            verify(glassTheme.loadBuiltin("dark"));
        }

        function test_blursWhatsBehindInTheSameWindow() {
            verify(overScenery.frosted);
            // Outside a compositor that blurs (here, none at all), it's solid.
            verify(!overApps.frosted);
        }

        function test_reduceTransparencyMakesItSolid() {
            glassTheme.reduceTransparency = true;
            verify(!overScenery.frosted);
        }

        function test_highContrastIsSolid() {
            verify(glassTheme.loadBuiltin("high-contrast"));
            verify(!overScenery.frosted);
        }

        function test_theBlurShowsThePictureThrough() {
            const image = grabImage(root);
            // Red under see-through dark glass: redder than the solid tint beside it.
            const frosted = image.pixel(130, 400);
            const solid = image.pixel(350, 400);
            verify(frosted.r > solid.r + 0.15, `frosted ${frosted} solid ${solid}`);
            // Blurred, the blue stripe spreads into the red just beside it.
            const beside = image.pixel(212, 400);
            verify(beside.b > frosted.b + 0.03, `beside ${beside} red ${frosted}`);
        }
    }
}
