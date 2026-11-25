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
    // How many whole rows show, before the one peeking in.
    property int rows: 1
    // Room around each tile for its ring and for growing when focused.
    readonly property int inset: Theme.tokens.sizes.gap

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
            picture: cell.picture
            text: cell.name

            onActiveFocusChanged: {
                if (activeFocus) {
                    grid.currentIndex = cell.index;
                }
            }
            onClicked: grid.tileClicked(cell.index)
        }
    }
}
