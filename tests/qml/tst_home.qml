// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtTest
import Hearth.Apps
import Hearth.Controls
import Hearth.Shell

Item {
    id: root

    function focusedName(): string {
        const tile = root.Window.activeFocusItem as Tile;
        return tile ? tile.text : "nothing";
    }

    height: 1080
    width: 1920

    ShellSettings {
        id: shellSettings
    }

    AppList {
        id: appList

        settings: shellSettings
    }

    AppList {
        id: noApps

        folders: []
    }

    HomeScreen {
        id: home

        anchors.fill: parent
        apps: appList
        focus: true
        settings: shellSettings
    }

    SignalSpy {
        id: chosen

        signalName: "appChosen"
        target: home
    }

    TestCase {
        property var folders: []

        function initTestCase() {
            // Whatever an earlier run left.
            folders = appList.folders;
            appList.folders = [];
            shellSettings.appOrder = [];
            appList.folders = folders;
        }

        function init() {
            home.apps = appList;
            home.forceActiveFocus();
            chosen.clear();
        }

        function test_1_startsOnTheFirstApp() {
            compare(appList.count, 7);
            tryVerify(() => root.focusedName() === "Test app 0", 1000, "focus is on " + root.focusedName());
        }

        function test_2_everyAppIsReachable() {
            for (let i = 0; i < 6; ++i) {
                keyClick(Qt.Key_Right);
            }
            compare(root.focusedName(), "Test app 6");
        }

        function test_3_selectChoosesTheFocusedApp() {
            keyClick(Qt.Key_Return);
            compare(chosen.count, 1);
            compare(chosen.signalArguments[0][0], 6);
        }

        function test_4_columnsSettingChangesTheGrid() {
            shellSettings.columns = 3;
            keyClick(Qt.Key_Up);
            compare(root.focusedName(), "Test app 3");
            shellSettings.columns = 5;
        }

        function test_7_moveMode() {
            home.goBack();
            compare(home.focusedApp, "Test app 0");
            home.startMoving();
            keyClick(Qt.Key_Right);
            keyClick(Qt.Key_Right);
            compare(root.focusedName(), "Test app 0");
            keyClick(Qt.Key_Down);
            // The second row isn't full, so it's as far as it goes.
            compare(appList.data(appList.index(6, 0), AppList.NameRole), "Test app 0");
            keyClick(Qt.Key_Return);
            verify(!home.moving);
            compare(chosen.count, 0);
            compare(shellSettings.appOrder[6], "app0");
            // And back, by Back.
            home.startMoving();
            keyClick(Qt.Key_Up);
            keyClick(Qt.Key_Left);
            keyClick(Qt.Key_Left);
            home.goBack();
            verify(!home.moving);
            compare(shellSettings.appOrder[0], "app0");
        }

        function test_8_upGoesToTheStatusBar() {
            home.goBack();
            keyClick(Qt.Key_Up);
            compare(root.Window.activeFocusItem.objectName, "bell");
            keyClick(Qt.Key_Down);
            compare(root.focusedName(), "Test app 0");
        }

        // Installing an app reloads the list. The remote must still be on a tile.
        function test_6_focusSurvivesAReload() {
            appList.folders = [];
            appList.folders = folders;
            tryVerify(() => root.focusedName().startsWith("Test app"), 1000, "focus is on " + root.focusedName());
        }

        function test_5_noAppsSaysSo() {
            home.apps = noApps;
            const message = findChild(home, "noApps");
            verify(message.visible);
            compare(message.text, "No apps are installed yet.");
        }

        name: "Home"
        when: windowShown
    }
}
