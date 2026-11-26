// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import Hearth.Theme

// Rows of tiles, moved through with the arrows. The focused row sits at the top
// with a little of the next row peeking in below it. Its model gives each tile a
// name and a picture.
GridView {
    id: grid

    property int columns: 5
    // The arrows move the focused tile instead of the focus.
    property bool arranging: false
    // How many whole rows show, before the one peeking in.
    property int rows: 1
    // Room around each tile for its ring and for growing when focused.
    readonly property int inset: Theme.tokens.sizes.gap

    signal moveEnded
    // The tile at FROM should go to TO.
    signal moveWanted(int from, int to)
    signal tileClicked(int index)

    cellHeight: Math.round((cellWidth - 2 * inset) * 0.625 + Theme.tokens.sizes.textSmall * 1.5 + 3 * inset)
    cellWidth: Math.floor(width / Math.max(columns, 1))
    // Rows that scroll out of view mustn't draw over what's above the grid.
    clip: true
    highlightFollowsCurrentItem: true
    highlightMoveDuration: Theme.tokens.reducedMotion ? 0 : Theme.tokens.motion.duration
    highlightRangeMode: GridView.StrictlyEnforceRange
    implicitHeight: Math.round(cellHeight * (rows + 0.3))
    // Moved by the remote only, never dragged.
    interactive: false
    keyNavigationEnabled: true
    keyNavigationWraps: false
    preferredHighlightBegin: 0
    preferredHighlightEnd: cellHeight * rows

    Keys.onPressed: event => {
        if (!grid.arranging) {
            return;
        }
        const steps = new Map([[Qt.Key_Left, -1], [Qt.Key_Right, 1], [Qt.Key_Up, -grid.columns], [Qt.Key_Down, grid.columns]]);
        const step = steps.get(event.key);
        if (step === undefined) {
            return;
        }
        event.accepted = true;
        let to = grid.currentIndex + step;
        if (event.key === Qt.Key_Down && to >= grid.count && Math.floor(grid.currentIndex / grid.columns) < Math.floor((grid.count - 1) / grid.columns)) {
            // The last row isn't full: the end of it is as far as it goes.
            to = grid.count - 1;
        }
        if (to >= 0 && to < grid.count) {
            grid.moveWanted(grid.currentIndex, to);
            // The view keeps its place in the list, not the tile: it follows.
            grid.currentIndex = to;
        }
    }
    // Something is always highlighted, also when the list arrives after the grid.
    onCountChanged: {
        if (currentIndex < 0 && count > 0) {
            currentIndex = 0;
        }
    }

    delegate: FocusScope {
        id: cell

        required property int index
        required property string name
        required property url picture

        height: grid.cellHeight
        width: grid.cellWidth
        z: tile.activeFocus ? 1 : 0

        Tile {
            id: tile

            anchors.fill: parent
            anchors.margins: grid.inset
            focus: true
            moving: grid.arranging && cell.index === grid.currentIndex
            picture: cell.picture
            text: cell.name

            onActiveFocusChanged: {
                if (activeFocus) {
                    grid.currentIndex = cell.index;
                }
            }
            onClicked: {
                if (grid.arranging) {
                    grid.moveEnded();
                } else {
                    grid.tileClicked(cell.index);
                }
            }
        }
    }
}
