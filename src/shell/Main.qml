// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import org.kde.ki18n
import Hearth.Apps
import Hearth.Controls
import Hearth.Failures
import Hearth.Overlays
import Hearth.Session
import Hearth.Theme

// The shell's window, and what ties it to the session around it.
Window {
    id: window

    // What the failure screen is showing, and the app "Try again" opens.
    property failureText failure
    property string failedApp
    // Inside a Hearth session, rather than in a window on a desktop.
    property bool inSession: false

    color: Theme.tokens.colors.background
    height: 1080
    title: KI18n.i18n("Hearth")
    visibility: window.inSession ? Window.FullScreen : Window.Windowed
    width: 1920

    function dismiss() {
        failureOverlay.shown = false;
        question.shown = false;
    }

    function fail(code: string, appId: string, name: string) {
        // On screen, in the log, and nowhere else.
        console.warn(code, name);
        window.failedApp = appId;
        window.failure = Failures.describe(code, name);
        failureOverlay.shown = true;
    }

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

        onCouldNotStart: (id, name) => window.fail("HEARTH-APP-001", id, name)
        onCrashed: (id, name) => window.fail("HEARTH-APP-003", id, name)
        onEndedEarly: (id, name) => window.fail("HEARTH-APP-002", id, name)
        onStillOpen: question.shown = true
    }

    CommandServer {
        id: server

        state: ({
                home: switcher.homeInFront,
                failure: failureOverlay.shown ? window.failure.code : "",
                question: question.shown,
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
        screenOpen: failureOverlay.shown || question.shown
        textBoxFocused: server.textBoxFocused

        onCloseKeyboard: server.hideKeyboard()
        onDeleteBackward: server.sendBackspace()
        onGoHome: {
            window.dismiss();
            switcher.goHome();
        }
        onHomeBack: homeScreen.goToFirstApp()
        onScreenBack: {
            // Back on the question about an app that won't close: leave it be.
            if (question.shown) {
                switcher.returnToApp();
            }
            window.dismiss();
        }
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

    Overlay {
        id: failureOverlay

        name: "failure"

        onVisibleChanged: {
            if (visible) {
                failureScreen.show();
            }
        }

        FailureScreen {
            id: failureScreen

            actions: [KI18n.i18n("Try again")]
            anchors.fill: parent
            failure: window.failure
            focus: true

            onActionChosen: {
                window.dismiss();
                switcher.openById(window.failedApp);
            }
            onDismissed: window.dismiss()
        }
    }

    Overlay {
        id: question

        name: "question"

        onVisibleChanged: {
            if (visible) {
                stillOpenDialog.show();
            }
        }

        Dialog {
            id: stillOpenDialog

            answers: [KI18n.i18n("Go back to it"), KI18n.i18n("Close it anyway")]
            focus: true
            text: KI18n.i18n("It may be asking whether to save something. Closing it anyway loses what isn't saved.")
            title: KI18n.i18n("%1 is still open", switcher.runningName)

            onAnswered: index => {
                window.dismiss();
                if (index === 0) {
                    switcher.returnToApp();
                } else {
                    switcher.forceClose();
                }
            }
        }
    }
}
