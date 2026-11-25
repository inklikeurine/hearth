// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import org.kde.ki18n
import Hearth.Controls
import Hearth.Theme

// The volume, for a moment after it changes.
Rectangle {
    id: bar

    property bool muted: false
    property int percent: 0
    property bool showing: false
    // Set when the volume couldn't be changed.
    property bool noOutput: false

    function show(noOutput: bool) {
        bar.noOutput = noOutput;
        bar.showing = true;
        timer.restart();
    }

    Accessible.name: value.text
    Accessible.role: Accessible.Indicator
    border.color: Theme.tokens.colors.outline
    border.width: 2
    color: Theme.tokens.colors.surface
    height: Theme.tokens.sizes.textBody * 2.5
    radius: Theme.tokens.sizes.radius
    visible: bar.showing
    width: row.width + 2 * Theme.tokens.sizes.gap

    Timer {
        id: timer

        interval: 2000

        onTriggered: bar.showing = false
    }

    Row {
        id: row

        anchors.centerIn: parent
        spacing: Theme.tokens.sizes.gap

        Icon {
            anchors.verticalCenter: parent.verticalCenter
            name: bar.muted || bar.noOutput ? "muted" : "volume"
        }

        Rectangle {
            Accessible.ignored: true
            anchors.verticalCenter: parent.verticalCenter
            color: Theme.tokens.colors.textDim
            height: 8
            radius: 4
            visible: !bar.noOutput
            width: Math.round(400 * Theme.tokens.screenHeight / 1080)

            Rectangle {
                color: bar.muted ? Theme.tokens.colors.textDim : Theme.tokens.colors.accent
                height: parent.height
                radius: 4
                width: parent.width * bar.percent / 100
            }
        }

        Label {
            id: value

            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideNone
            objectName: "value"
            text: {
                if (bar.noOutput) {
                    return KI18n.i18n("No sound output found");
                }
                return bar.muted ? KI18n.i18n("Muted") : KI18n.i18n("%1%", bar.percent);
            }
        }
    }
}
