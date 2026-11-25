// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import org.kde.ki18n
import Hearth.Apps
import Hearth.Theme

// The shell's window.
Window {
    id: window

    color: Theme.tokens.colors.background
    height: 1080
    title: KI18n.i18n("Hearth")
    visible: true
    width: 1920

    Binding {
        property: "screenHeight"
        target: window.Theme.tokens
        value: window.height
    }

    ShellSettings {
        id: shellSettings
    }

    AppList {
        id: appList

        settings: shellSettings
    }

    AppLauncher {
        id: launcher

        // The failure screen replaces this when it's built (roadmap, milestone 1).
        onFailed: (appId, name) => console.warn("couldn't start", appId, name)
    }

    HomeScreen {
        anchors.fill: parent
        apps: appList
        focus: true
        settings: shellSettings

        onAppChosen: index => launcher.launch(appList, index)
    }
}
