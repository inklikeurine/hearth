// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtTest
import Hearth.Controls

Item {
    id: root

    function focusedName(): string {
        const tile = root.Window.activeFocusItem as Tile;
        return tile ? tile.text : "nothing";
    }

    height: 1080
    width: 1920

    TileGrid {
        id: grid

        focus: true
        model: names
        width: parent.width
    }

    ListModel {
        id: names

        Component.onCompleted: {
            for (let i = 0; i < 13; ++i) {
                append({
                    name: "App " + i,
                    picture: ""
                });
            }
        }
    }

    SignalSpy {
        id: clicks

        signalName: "tileClicked"
        target: grid
    }

    TestCase {
        function init() {
            grid.columns = 5;
            grid.rows = 1;
            grid.currentIndex = 0;
            grid.forceActiveFocus();
            clicks.clear();
        }

        // Every tile can be reached, the last in each row included.
        function test_everyTileIsReachable() {
            const seen = [];
            for (let i = 0; i < names.count; ++i) {
                seen.push(root.focusedName());
                keyClick(Qt.Key_Right);
            }
            compare(seen, Array.from({
                length: 13
            }, (unused, i) => "App " + i));
        }

        function test_downAndUp() {
            keyClick(Qt.Key_Down);
            compare(root.focusedName(), "App 5");
            keyClick(Qt.Key_Down);
            compare(root.focusedName(), "App 10");
            keyClick(Qt.Key_Up);
            compare(root.focusedName(), "App 5");
        }

        function test_edgesKeepFocus() {
            keyClick(Qt.Key_Left);
            compare(root.focusedName(), "App 0");
            keyClick(Qt.Key_Up);
            compare(root.focusedName(), "App 0");
        }

        function test_focusedRowComesToTheTop() {
            keyClick(Qt.Key_Down);
            tryCompare(grid, "contentY", grid.cellHeight);
        }

        function test_nextRowPeeksIn() {
            verify(grid.height > grid.cellHeight);
            verify(grid.height < 2 * grid.cellHeight);
            grid.rows = 2;
            verify(grid.height > 2 * grid.cellHeight);
        }

        function test_columnsSetting() {
            grid.columns = 3;
            keyClick(Qt.Key_Down);
            compare(root.focusedName(), "App 3");
        }

        function test_selectReportsTheTile() {
            keyClick(Qt.Key_Right);
            keyClick(Qt.Key_Return);
            compare(clicks.count, 1);
            compare(clicks.signalArguments[0][0], 1);
        }

        function test_clickedTileBecomesCurrent() {
            mouseClick(grid, grid.cellWidth * 2.5, grid.cellHeight / 3);
            compare(grid.currentIndex, 2);
            compare(clicks.signalArguments[0][0], 2);
        }

        name: "TileGrid"
        when: windowShown
    }
}
