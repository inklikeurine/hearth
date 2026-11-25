// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Controls
import Hearth.Theme

// A notification that's just arrived, for a few seconds.
Rectangle {
    id: popup

    property string app
    property string body
    property bool showing: false
    property string summary

    function show(app: string, summary: string, body: string) {
        popup.app = app;
        popup.summary = summary;
        popup.body = body;
        popup.showing = true;
        timer.restart();
    }

    Accessible.description: popup.body
    Accessible.name: popup.summary
    Accessible.role: Accessible.Notification
    border.color: Theme.tokens.colors.outline
    border.width: 2
    color: Theme.tokens.colors.surface
    height: words.height + 2 * Theme.tokens.sizes.gap
    radius: Theme.tokens.sizes.radius
    visible: popup.showing
    width: Math.round(640 * Theme.tokens.screenHeight / 1080)

    Timer {
        id: timer

        interval: 5000

        onTriggered: popup.showing = false
    }

    Column {
        id: words

        anchors.left: parent.left
        anchors.margins: Theme.tokens.sizes.gap
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        Label {
            color: Theme.tokens.colors.textDim
            font.pixelSize: Theme.tokens.sizes.textSmall
            text: popup.app
            visible: text.length > 0
            width: parent.width
        }

        Label {
            text: popup.summary
            width: parent.width
        }

        Label {
            font.pixelSize: Theme.tokens.sizes.textSmall
            maximumLineCount: 3
            text: popup.body
            visible: text.length > 0
            width: parent.width
            wrapMode: Text.Wrap
        }
    }
}
