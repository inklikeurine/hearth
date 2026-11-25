// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Theme

// A black screen with a dim clock that never stays in one place.
FocusScope {
    id: saver

    signal dismissed

    // Somewhere else on the screen.
    function move() {
        clock.x = Math.round(Math.random() * Math.max(0, saver.width - clock.width));
        clock.y = Math.round(Math.random() * Math.max(0, saver.height - clock.height));
    }

    Keys.onPressed: event => {
        event.accepted = true;
        saver.dismissed();
    }
    onVisibleChanged: {
        if (visible) {
            saver.move();
        }
    }

    Rectangle {
        Accessible.ignored: true
        anchors.fill: parent
        color: "black"
    }

    Clock {
        id: clock

        color: "#8a8a8a"
        font.pixelSize: Theme.tokens.sizes.textTitle * 2
    }

    Timer {
        interval: 30000
        repeat: true
        running: saver.visible

        onTriggered: saver.move()
    }

    MouseArea {
        anchors.fill: parent

        onClicked: saver.dismissed()
    }
}
