// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import org.kde.ki18n
import Hearth.Apps
import Hearth.Controls
import Hearth.Panels
import Hearth.Theme

// The desktop the apps sit on: the background, and the profile's apps in rows
// along the bottom.
FocusScope {
    id: home

    required property AppList apps
    // An app that's been chosen and hasn't appeared yet.
    property string openingName
    readonly property string focusedApp: {
        const index = home.apps.index(grid.currentIndex, 0);
        return grid.currentIndex >= 0 && grid.currentIndex < home.apps.count ? home.apps.data(index, AppList.NameRole) : "";
    }
    // The arrows move the focused app.
    property alias moving: grid.arranging
    property alias network: status.network
    property alias profileName: status.profileName
    property bool resting: false
    required property ShellSettings settings
    property alias unseen: status.unseen

    signal appChosen(int index)
    signal bellPressed
    signal powerPressed

    function startMoving() {
        grid.forceActiveFocus();
        home.moving = home.focusedApp.length > 0;
    }

    // Where Back goes: out of moving, then to the first app.
    function goBack() {
        if (home.moving) {
            home.moving = false;
            return;
        }
        grid.currentIndex = 0;
        grid.forceActiveFocus();
    }

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

    StatusBar {
        id: status

        KeyNavigation.down: grid
        anchors.right: parent.right
        anchors.rightMargin: Theme.tokens.sizes.safeSide
        anchors.top: parent.top
        anchors.topMargin: Theme.tokens.sizes.safeVertical
        resting: home.resting

        onBellPressed: home.bellPressed()
        onPowerPressed: home.powerPressed()
    }

    TileGrid {
        id: grid

        KeyNavigation.priority: KeyNavigation.AfterItem
        KeyNavigation.up: status

        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.tokens.sizes.safeVertical
        columns: home.settings.columns
        focus: true
        model: home.apps
        rows: home.settings.rows
        width: parent.width - 2 * (Theme.tokens.sizes.safeSide - inset)
        x: Theme.tokens.sizes.safeSide - inset

        onMoveEnded: home.moving = false
        onMoveWanted: (from, to) => home.apps.moveApp(from, to)
        onTileClicked: index => home.appChosen(index)
    }

    Label {
        anchors.bottom: grid.top
        anchors.bottomMargin: Theme.tokens.sizes.gap
        anchors.horizontalCenter: parent.horizontalCenter
        text: home.moving ? KI18n.i18n("Move %1 with the arrows, then press Select.", home.focusedApp) : KI18n.i18n("Opening %1…", home.openingName)
        visible: home.moving || home.openingName.length > 0
    }

    Label {
        anchors.centerIn: parent
        objectName: "noApps"
        text: KI18n.i18n("No apps are installed yet.")
        visible: home.apps.count === 0
    }
}
