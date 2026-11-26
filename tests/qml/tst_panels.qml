// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import QtTest
import Hearth.Controls
import Hearth.Panels
import Hearth.System

Item {
    id: root

    height: 1080
    width: 1920

    Notifications {
        id: service
    }

    SystemMenu {
        id: menu

        anchors.fill: parent
        visible: false
    }

    NotificationList {
        id: list

        anchors.fill: parent
        notifications: service
        visible: false
    }

    StatusBar {
        id: bar

        x: 1000
        y: 100
    }

    VolumeBar {
        id: volumeBar
    }

    Screensaver {
        id: saver

        anchors.fill: parent
        visible: false
    }

    BurnInGuard {
        id: guard

        Rectangle {
            id: guarded

            height: 50
            width: 50
        }
    }

    SignalSpy {
        id: spy
    }

    TestCase {
        function texts(panel: Item): list<string> {
            const found = [];
            const visit = item => {
                if (item instanceof ListRow && item.visible) {
                    found.push(item.text);
                }
                Array.from(item.children).forEach(visit);
            };
            visit(panel);
            return found;
        }

        function focusedText(): string {
            const row = root.Window.activeFocusItem as ListRow;
            return row ? row.text : "nothing";
        }

        function watch(target: Item, signalName: string) {
            spy.target = null;
            spy.signalName = signalName;
            spy.target = target;
            spy.clear();
        }

        function cleanup() {
            menu.visible = false;
            list.visible = false;
            saver.visible = false;
            menu.appItems = [];
        }

        function test_menuAtHome() {
            menu.atHome = true;
            menu.appName = "";
            menu.focusedApp = "Films";
            menu.canPlayPause = false;
            menu.visible = true;
            menu.show();
            compare(texts(menu), ["Move Films", "Notifications", "Settings", "Power"]);
            compare(focusedText(), "Move Films");
        }

        function test_menuInAnApp() {
            menu.atHome = false;
            menu.appName = "Films";
            menu.canPlayPause = true;
            menu.playing = true;
            menu.unseen = 2;
            menu.visible = true;
            menu.show();
            compare(texts(menu), ["Pause", "Go home", "Close Films", "Notifications (2 new)", "Settings", "Power"]);
            menu.unseen = 0;
            watch(menu, "closeAppChosen");
            keyClick(Qt.Key_Down);
            keyClick(Qt.Key_Down);
            keyClick(Qt.Key_Return);
            compare(spy.count, 1);
        }

        function test_menuShowsWhatTheAppAdded() {
            menu.atHome = false;
            menu.appName = "Films";
            menu.canPlayPause = false;
            menu.appItems = [
                {
                    id: "next",
                    type: "action",
                    label: "Next <b>episode</b>",
                    enabled: true
                },
                {
                    id: "off",
                    type: "action",
                    label: "Not now",
                    enabled: false
                },
                {
                    id: "subtitles",
                    type: "toggle",
                    label: "Subtitles",
                    on: true,
                    enabled: true
                },
                {
                    id: "audio",
                    type: "choice",
                    label: "Audio",
                    options: ["English", "French"],
                    current: 1,
                    enabled: true
                }
            ];
            menu.visible = true;
            menu.show();
            // Markup stays text.
            compare(focusedText(), "Next <b>episode</b>");
            watch(menu, "appItemChosen");
            keyClick(Qt.Key_Return);
            compare(spy.signalArguments[0][0], "next");
            // One that can't be chosen is passed over.
            keyClick(Qt.Key_Down);
            compare(focusedText(), "Subtitles");
            keyClick(Qt.Key_Return);
            compare(spy.signalArguments[1][1], false);
            keyClick(Qt.Key_Down);
            keyClick(Qt.Key_Return);
            compare(spy.signalArguments[2][1], 0);
            menu.atHome = true;
            verify(!texts(menu).includes("Subtitles"));
        }

        function test_clickBesideThePanelClosesIt() {
            menu.visible = true;
            watch(menu, "dismissed");
            mouseClick(root, 100, 900);
            compare(spy.count, 1);
            mouseClick(root, 1800, 1060);
            compare(spy.count, 1);
        }

        function test_notifications() {
            list.visible = true;
            list.show();
            compare(texts(list), ["Close"]);
            verify(findChild(list, "nothing").visible);
        }

        function test_statusBarNetwork_data() {
            return [
                {
                    tag: "wired",
                    kind: NetworkState.Wired,
                    icon: "wired"
                },
                {
                    tag: "wifi",
                    kind: NetworkState.Wifi,
                    icon: "wifi"
                },
                {
                    tag: "none",
                    kind: NetworkState.None,
                    icon: "none"
                }
            ];
        }

        function test_statusBarNetwork(data: var) {
            bar.network = data.kind;
            const icon = findChild(bar, "network");
            verify(icon.visible);
            compare(icon.name, data.icon);
            verify(icon.Accessible.name.length > 0);
            bar.network = NetworkState.Unknown;
            verify(!icon.visible);
        }

        function test_statusBarButtons() {
            bar.forceActiveFocus();
            verify(findChild(bar, "bell").activeFocus);
            watch(bar, "bellPressed");
            keyClick(Qt.Key_Return);
            compare(spy.count, 1);
            keyClick(Qt.Key_Right);
            watch(bar, "powerPressed");
            keyClick(Qt.Key_Return);
            compare(spy.count, 1);
        }

        function test_volume() {
            volumeBar.percent = 45;
            volumeBar.muted = false;
            volumeBar.show(false);
            verify(volumeBar.visible);
            compare(findChild(volumeBar, "value").text, "45%");
            volumeBar.muted = true;
            compare(findChild(volumeBar, "value").text, "Muted");
            volumeBar.show(true);
            compare(findChild(volumeBar, "value").text, "No sound output found");
            tryVerify(() => !volumeBar.visible, 3000);
        }

        function test_screensaverEndsOnAnyKey() {
            saver.visible = true;
            saver.forceActiveFocus();
            watch(saver, "dismissed");
            keyClick(Qt.Key_F7);
            compare(spy.count, 1);
            mouseClick(saver, 10, 10);
            compare(spy.count, 2);
        }

        function test_screensaverClockMoves() {
            const clock = Array.from(saver.children).find(child => child instanceof Clock);
            const places = new Set();
            for (let i = 0; i < 10; ++i) {
                saver.move();
                verify(clock.x >= 0 && clock.x + clock.width <= saver.width);
                verify(clock.y >= 0 && clock.y + clock.height <= saver.height);
                places.add(clock.x + "," + clock.y);
            }
            verify(places.size > 5);
        }

        function test_burnInGuardMovesAFewPixels() {
            const places = new Set();
            for (let i = 0; i < 8; ++i) {
                guard.step = i;
                const place = guarded.mapToItem(guard, 0, 0);
                verify(Math.abs(place.x) <= guard.reach && Math.abs(place.y) <= guard.reach);
                places.add(place.x + "," + place.y);
            }
            verify(places.size >= 6);
        }

        name: "Panels"
        when: windowShown
    }
}
