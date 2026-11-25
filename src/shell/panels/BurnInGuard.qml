// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Theme

// Keeps something that's always on screen from marking it: it moves a few pixels
// every minute, and dims once nobody has pressed anything for a while.
Item {
    id: guard

    default property alias content: inner.data
    // Nobody has pressed anything for a while.
    property bool resting: false
    // Pixels it may move each way.
    readonly property int reach: Math.max(2, Math.round(4 * Theme.tokens.screenHeight / 1080))
    property int step: 0

    implicitHeight: inner.childrenRect.height
    implicitWidth: inner.childrenRect.width

    Timer {
        interval: 60000
        repeat: true
        running: guard.visible

        onTriggered: guard.step = (guard.step + 1) % 8
    }

    Item {
        id: inner

        height: guard.height
        opacity: guard.resting ? 0.7 : 1
        width: guard.width
        // A small circle, one place a minute.
        x: Math.round(guard.reach * Math.cos(guard.step * Math.PI / 4))
        y: Math.round(guard.reach * Math.sin(guard.step * Math.PI / 4))

        Behavior on opacity {
            NumberAnimation {
                duration: guard.Theme.tokens.motion.duration * 5
            }
        }
    }
}
