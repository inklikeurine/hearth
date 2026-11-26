// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Controls
import Hearth.Theme

// A panel down the right side, over whatever is showing, which it dims.
FocusScope {
    id: panel

    // What's under the panel in the same window, for Qt to blur. Without it the
    // compositor is asked to, and the panel is solid where it can't.
    property Item behind: null
    default property alias content: column.content
    property string title

    signal dismissed

    function show() {
        column.current = null;
        column.start();
    }

    Accessible.name: panel.title
    Accessible.role: Accessible.Pane

    Rectangle {
        Accessible.ignored: true
        anchors.fill: parent
        color: Qt.alpha(panel.Theme.tokens.colors.background, 0.6)
    }

    // A click beside the panel closes it.
    MouseArea {
        anchors.fill: parent

        onClicked: panel.dismissed()
    }

    Item {
        anchors.right: parent.right
        height: parent.height
        width: Math.round(parent.height * 0.75)

        PanelBackground {
            anchors.fill: parent
            source: panel.behind
        }

        MouseArea {
            anchors.fill: parent
        }

        Label {
            id: heading

            anchors.left: parent.left
            anchors.leftMargin: Theme.tokens.sizes.gap * 2
            anchors.right: clock.left
            anchors.rightMargin: Theme.tokens.sizes.gap
            anchors.top: parent.top
            anchors.topMargin: Theme.tokens.sizes.safeVertical
            font.pixelSize: Theme.tokens.sizes.textTitle
            text: panel.title
        }

        Clock {
            id: clock

            anchors.baseline: heading.baseline
            anchors.right: parent.right
            anchors.rightMargin: Theme.tokens.sizes.safeSide
            color: Theme.tokens.colors.textDim
        }

        FocusColumn {
            id: column

            anchors.bottom: parent.bottom
            anchors.bottomMargin: Theme.tokens.sizes.safeVertical
            anchors.left: parent.left
            anchors.leftMargin: Theme.tokens.sizes.gap * 2 - inset
            anchors.right: parent.right
            anchors.rightMargin: Theme.tokens.sizes.safeSide - inset
            anchors.top: heading.bottom
            anchors.topMargin: Theme.tokens.sizes.gap
            focus: true
        }
    }
}
