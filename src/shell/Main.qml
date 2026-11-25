// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import org.kde.ki18n
import Hearth.Apps
import Hearth.Controls
import Hearth.Failures
import Hearth.Overlays
import Hearth.Panels
import Hearth.Session
import Hearth.Settings
import Hearth.System
import Hearth.Theme

// The shell's window, and what ties it to the session around it.
Window {
    id: window

    // What the failure screen is showing, and the app "Try again" opens.
    property failureText failure
    property string failedApp
    // Inside a Hearth session, rather than in a window on a desktop.
    property bool inSession: false
    property bool settingsOpen: false

    // Every screen over what's in front goes, so the next one is alone.
    function dismiss() {
        failureOverlay.shown = false;
        menuOverlay.shown = false;
        notificationOverlay.shown = false;
        powerOverlay.shown = false;
        question.shown = false;
    }

    function fail(code: string, appId: string, name: string) {
        // On screen, in the log, and nowhere else.
        console.warn(code, name);
        window.dismiss();
        window.failedApp = appId;
        window.failure = Failures.describe(code, name);
        failureOverlay.shown = true;
    }

    function handle(command: string) {
        resting.activity();
        saverWatch.activity();
        if (saverOverlay.shown) {
            // The press that ends the screensaver does nothing else.
            saverOverlay.shown = false;
            return;
        }
        if (!presses.allows(command)) {
            return;
        }
        switch (command) {
        case "volume-up":
            volume.up();
            break;
        case "volume-down":
            volume.down();
            break;
        case "volume-mute":
            volume.toggleMute();
            break;
        default:
            router.handle(command);
        }
    }

    function openSettings() {
        window.dismiss();
        switcher.goHome();
        window.settingsOpen = true;
        settingsScreen.show();
    }

    function showHome() {
        window.settingsOpen = false;
        homeScreen.moving = false;
        homeScreen.forceActiveFocus();
    }

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

    Binding {
        property: "textScale"
        target: window.Theme.tokens
        value: shellSettings.textPercent / 100
    }

    Binding {
        property: "reducedMotion"
        target: window.Theme.tokens
        value: shellSettings.reducedMotion
    }

    ShellSettings {
        id: shellSettings

        readonly property var keys: [shellSettings.extraKeys, shellSettings.repeatSpeed]
        property string usedTheme

        function useTheme() {
            if (shellSettings.theme !== shellSettings.usedTheme) {
                shellSettings.usedTheme = shellSettings.theme;
                if (!window.Theme.tokens.loadBuiltin(shellSettings.theme)) {
                    // A theme this version doesn't have.
                    window.Theme.tokens.loadBuiltin("dark");
                }
            }
        }

        Component.onCompleted: shellSettings.useTheme()
        onChanged: shellSettings.useTheme()
        onKeysChanged: labwc.apply()
    }

    LabwcConfig {
        id: labwc

        settings: shellSettings
    }

    PressFilter {
        id: presses

        active: shellSettings.ignoreDoublePresses
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
        onStillOpen: {
            window.dismiss();
            question.shown = true;
        }
    }

    MenuContributions {
        id: contributions

        launcher: appLauncher
    }

    MediaPlayers {
        id: players
    }

    NetworkState {
        id: networkState
    }

    Power {
        id: power

        onFailed: window.fail("HEARTH-POWER-001", "", "")
    }

    Volume {
        id: volume

        onFailed: volumeBar.show(true)
        onShown: volumeBar.show(false)
    }

    Notifications {
        id: notificationService

        onArrived: (app, summary, body) => popup.show(app, summary, body)
    }

    // For dimming what's always on screen.
    IdleWatcher {
        id: resting

        seconds: 60
    }

    // The home screen and settings only: what's in an app may be a film.
    IdleWatcher {
        id: saverWatch

        running: switcher.homeInFront && !failureOverlay.shown
        seconds: shellSettings.screensaverMinutes * 60

        onIdleChanged: saverOverlay.shown = idle
    }

    CommandServer {
        id: server

        state: ({
                failure: failureOverlay.shown ? window.failure.code : "",
                home: switcher.homeInFront,
                keyboard: server.keyboardShown,
                menu: menuOverlay.shown,
                moving: homeScreen.moving,
                notifications: notificationOverlay.shown,
                opening: switcher.openingName,
                power: powerOverlay.shown,
                question: question.shown,
                running: switcher.runningId,
                screensaver: saverOverlay.shown,
                settings: window.settingsOpen,
                textBox: server.textBoxFocused,
                theme: shellSettings.usedTheme,
                volume: volumeBar.showing
            })

        onCommand: name => window.handle(name)
    }

    KeyRouter {
        id: router

        homeInFront: switcher.homeInFront
        keyboardShown: server.keyboardShown
        menuOpen: menuOverlay.shown
        screenOpen: failureOverlay.shown || question.shown || notificationOverlay.shown || powerOverlay.shown
        textBoxFocused: server.textBoxFocused

        onCloseKeyboard: server.hideKeyboard()
        onCloseMenu: menuOverlay.shown = false
        onDeleteBackward: server.sendBackspace()
        onGoHome: {
            window.dismiss();
            switcher.goHome();
            window.showHome();
        }
        onHomeBack: {
            if (window.settingsOpen) {
                settingsScreen.goBack();
            } else {
                homeScreen.goBack();
            }
        }
        onOpenMenu: {
            window.dismiss();
            homeScreen.moving = false;
            players.look();
            menuOverlay.shown = true;
        }
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
        enabled: !window.settingsOpen
        focus: !window.settingsOpen
        network: networkState.kind
        openingName: switcher.openingName
        resting: resting.idle
        settings: shellSettings
        unseen: notificationService.unseen
        visible: !window.settingsOpen

        onAppChosen: index => switcher.open(index)
        onBellPressed: notificationOverlay.shown = true
        onPowerPressed: powerOverlay.shown = true
    }

    SettingsScreen {
        id: settingsScreen

        anchors.fill: parent
        enabled: window.settingsOpen
        focus: window.settingsOpen
        settings: shellSettings
        visible: window.settingsOpen

        onClosed: window.showHome()
    }

    Overlay {
        id: menuOverlay

        name: "menu"

        onVisibleChanged: {
            if (visible) {
                systemMenu.show();
            }
        }

        SystemMenu {
            id: systemMenu

            anchors.fill: parent
            appItems: contributions.appId === switcher.runningId ? contributions.items : []
            appName: switcher.runningName
            atHome: switcher.homeInFront
            canPlayPause: players.available
            focus: true
            focusedApp: window.settingsOpen ? "" : homeScreen.focusedApp
            playing: players.playing
            unseen: notificationService.unseen

            onAppItemChosen: (id, value) => {
                contributions.choose(id, value);
                window.dismiss();
            }
            onCloseAppChosen: {
                window.dismiss();
                switcher.closeApp();
            }
            onDismissed: window.dismiss()
            onHomeChosen: router.handle("home")
            onMoveChosen: {
                window.dismiss();
                homeScreen.startMoving();
            }
            onNotificationsChosen: {
                window.dismiss();
                notificationOverlay.shown = true;
            }
            onPlayPauseChosen: {
                players.playPause();
                window.dismiss();
            }
            onPowerChosen: {
                window.dismiss();
                powerOverlay.shown = true;
            }
            onSettingsChosen: window.openSettings()
        }
    }

    Overlay {
        id: notificationOverlay

        name: "notifications"

        onVisibleChanged: {
            notificationService.markSeen();
            if (visible) {
                notificationList.show();
            }
        }

        NotificationList {
            id: notificationList

            anchors.fill: parent
            focus: true
            notifications: notificationService

            onDismissed: window.dismiss()
        }
    }

    Overlay {
        id: powerOverlay

        name: "power"

        onVisibleChanged: {
            if (visible) {
                powerDialog.show();
            }
        }

        Dialog {
            id: powerDialog

            answers: [KI18n.i18n("Sleep"), KI18n.i18n("Restart"), KI18n.i18n("Turn off"), KI18n.i18n("Cancel")]
            focus: true
            title: KI18n.i18n("Power")

            onAnswered: index => {
                window.dismiss();
                [() => power.sleep(), () => power.restart(), () => power.turnOff(), () => {}][index]();
            }
        }
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

            actions: window.failedApp.length > 0 ? [KI18n.i18n("Try again")] : []
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

    // What only shows for a moment, and takes no keys.
    Overlay {
        name: "brief"
        shown: volumeBar.showing || popup.showing
        takesKeyboard: false

        VolumeBar {
            id: volumeBar

            anchors.bottom: parent.bottom
            anchors.bottomMargin: Theme.tokens.sizes.safeVertical
            anchors.horizontalCenter: parent.horizontalCenter
            muted: volume.muted
            percent: volume.percent
        }

        NotificationPopup {
            id: popup

            anchors.margins: Theme.tokens.sizes.safeVertical
            anchors.right: parent.right
            anchors.rightMargin: Theme.tokens.sizes.safeSide
            anchors.top: parent.top
        }
    }

    Overlay {
        id: saverOverlay

        name: "screensaver"

        Screensaver {
            anchors.fill: parent
            focus: true

            onDismissed: saverOverlay.shown = false
        }
    }
}
