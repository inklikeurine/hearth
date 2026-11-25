// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import org.kde.ki18n
import Hearth.Apps
import Hearth.Controls
import Hearth.Theme

// The desktop the apps sit on: the background, and the profile's apps in rows
// along the bottom.
FocusScope {
    id: home

    required property AppList apps
    required property ShellSettings settings

    signal appChosen(int index)

    Image {
        id: background

        Accessible.ignored: true
        anchors.fill: parent
        asynchronous: true
        fillMode: Image.PreserveAspectCrop
        source: home.settings.background
        sourceSize: Qt.size(width, height)
        visible: status === Image.Ready
    }

    // Keeps names readable over any picture.
    Rectangle {
        Accessible.ignored: true
        anchors.bottom: parent.bottom
        height: grid.height + Theme.tokens.sizes.safeVertical * 3
        visible: background.visible
        width: parent.width

        gradient: Gradient {
            GradientStop {
                color: "transparent"
                position: 0.0
            }
            GradientStop {
                color: Qt.alpha(home.Theme.tokens.colors.background, 0.85)
                position: 0.3
            }
            GradientStop {
                color: home.Theme.tokens.colors.background
                position: 1.0
            }
        }
    }

    TileGrid {
        id: grid

        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.tokens.sizes.safeVertical
        columns: home.settings.columns
        focus: true
        model: home.apps
        rows: home.settings.rows
        width: parent.width - 2 * (Theme.tokens.sizes.safeSide - inset)
        x: Theme.tokens.sizes.safeSide - inset

        onTileClicked: index => home.appChosen(index)
    }

    Label {
        anchors.centerIn: parent
        text: KI18n.i18n("No apps are installed yet.")
        visible: home.apps.count === 0
    }
}
