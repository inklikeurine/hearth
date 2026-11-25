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
            folders = appList.folders;
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

        // Installing an app reloads the list. The remote must still be on a tile.
        function test_6_focusSurvivesAReload() {
            appList.folders = [];
            appList.folders = folders;
            tryVerify(() => root.focusedName().startsWith("Test app"), 1000, "focus is on " + root.focusedName());
        }

        function test_5_noAppsSaysSo() {
            home.apps = noApps;
            const message = Array.from(home.children).find(child => child instanceof Label);
            verify(message.visible);
            compare(message.text, "No apps are installed yet.");
        }

        name: "Home"
        when: windowShown
    }
}
