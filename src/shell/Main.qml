// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import org.kde.ki18n
import Hearth.Apps
import Hearth.Session
import Hearth.Theme

// The shell's window, and what ties it to the session around it.
Window {
    id: window

    // Inside a Hearth session, rather than in a window on a desktop.
    property bool inSession: false

    color: Theme.tokens.colors.background
    height: 1080
    title: KI18n.i18n("Hearth")
    visibility: window.inSession ? Window.FullScreen : Window.Windowed
    width: 1920

    Component.onCompleted: {
        if (window.inSession) {
            server.listen();
        }
    }

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
        id: appLauncher
    }

    WaylandWindows {
        id: sessionWindows
    }

    AppSwitcher {
        id: switcher

        apps: appList
        launcher: appLauncher
        windows: sessionWindows

        // The failure screen replaces these when it's built (roadmap, milestone 1).
        onCouldNotStart: name => console.warn("couldn't start", name)
        onCrashed: name => console.warn("crashed:", name)
        onEndedEarly: name => console.warn("ended early:", name)
        onStillOpen: name => console.warn("still open:", name)
    }

    CommandServer {
        id: server

        state: ({
                home: switcher.homeInFront,
                running: switcher.runningId,
                opening: switcher.openingName,
                keyboard: server.keyboardShown,
                textBox: server.textBoxFocused
            })

        onCommand: name => router.handle(name)
    }

    KeyRouter {
        id: router

        homeInFront: switcher.homeInFront
        keyboardShown: server.keyboardShown
        textBoxFocused: server.textBoxFocused

        onCloseKeyboard: server.hideKeyboard()
        onDeleteBackward: server.sendBackspace()
        onGoHome: switcher.goHome()
        onHomeBack: homeScreen.goToFirstApp()
    }

    HomeScreen {
        id: homeScreen

        anchors.fill: parent
        apps: appList
        focus: true
        openingName: switcher.openingName
        settings: shellSettings

        onAppChosen: index => switcher.open(index)
    }
}
