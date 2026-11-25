// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import org.kde.ki18n
import Hearth.Controls
import Hearth.Theme

// What the Menu button opens: what the app in front added, then what's always
// there.
SidePanel {
    id: menu

    // What the app in front added: MenuContributions' items.
    property var appItems: []
    property string appName
    property bool atHome: true
    property bool canPlayPause: false
    // The app the home screen is on, which can be moved.
    property string focusedApp
    property bool playing: false
    property int unseen: 0

    signal appItemChosen(string id, var value)
    signal closeAppChosen
    signal homeChosen
    signal moveChosen
    signal notificationsChosen
    signal playPauseChosen
    signal powerChosen
    signal settingsChosen

    title: menu.atHome || menu.appName.length === 0 ? KI18n.i18n("Menu") : menu.appName

    Repeater {
        model: menu.atHome ? [] : menu.appItems

        ListRow {
            id: appRow

            required property var modelData

            Accessible.checked: modelData.type === "toggle" && modelData.on
            detail: {
                if (appRow.modelData.type === "toggle") {
                    return appRow.modelData.on ? KI18n.i18n("On") : KI18n.i18n("Off");
                }
                return appRow.modelData.type === "choice" ? appRow.modelData.options[appRow.modelData.current] : "";
            }
            enabled: appRow.modelData.enabled
            objectName: "app-" + appRow.modelData.id
            text: appRow.modelData.label
            width: parent?.width ?? 0

            // What it shows only changes when the app says so.
            onClicked: {
                const item = appRow.modelData;
                if (item.type === "toggle") {
                    menu.appItemChosen(item.id, !item.on);
                } else if (item.type === "choice") {
                    menu.appItemChosen(item.id, (item.current + 1) % item.options.length);
                } else {
                    menu.appItemChosen(item.id, true);
                }
            }
        }
    }

    ListRow {
        iconName: menu.playing ? "pause" : "play"
        objectName: "playPause"
        text: menu.playing ? KI18n.i18n("Pause") : KI18n.i18n("Play")
        visible: menu.canPlayPause
        width: parent.width

        onClicked: menu.playPauseChosen()
    }

    ListRow {
        iconName: "home"
        objectName: "home"
        text: KI18n.i18n("Go home")
        visible: !menu.atHome
        width: parent.width

        onClicked: menu.homeChosen()
    }

    ListRow {
        iconName: "close"
        objectName: "close"
        text: KI18n.i18n("Close %1", menu.appName)
        visible: menu.appName.length > 0
        width: parent.width

        onClicked: menu.closeAppChosen()
    }

    ListRow {
        iconName: "move"
        objectName: "move"
        text: KI18n.i18n("Move %1", menu.focusedApp)
        visible: menu.atHome && menu.focusedApp.length > 0
        width: parent.width

        onClicked: menu.moveChosen()
    }

    ListRow {
        iconName: "bell"
        objectName: "notifications"
        text: menu.unseen > 0 ? KI18n.i18np("Notifications (1 new)", "Notifications (%1 new)", menu.unseen) : KI18n.i18n("Notifications")
        width: parent.width

        onClicked: menu.notificationsChosen()
    }

    ListRow {
        iconName: "settings"
        objectName: "settings"
        text: KI18n.i18n("Settings")
        width: parent.width

        onClicked: menu.settingsChosen()
    }

    ListRow {
        iconName: "power"
        objectName: "power"
        text: KI18n.i18n("Power")
        width: parent.width

        onClicked: menu.powerChosen()
    }
}
