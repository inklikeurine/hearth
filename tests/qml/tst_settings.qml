// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import QtTest
import Hearth.Apps
import Hearth.Settings
import Hearth.Theme

Item {
    id: root

    height: 1080
    width: 1920

    ShellSettings {
        id: shellSettings
    }

    ThemeTokens {
        id: contrast
    }

    SettingsScreen {
        id: screen

        anchors.fill: parent
        settings: shellSettings
    }

    SignalSpy {
        id: closed

        signalName: "closed"
        target: screen
    }

    TestCase {
        function control(name: string): Item {
            return findChild(screen, name);
        }

        function init() {
            shellSettings.theme = "dark";
            shellSettings.textPercent = 100;
            shellSettings.reducedMotion = false;
            shellSettings.columns = 5;
            shellSettings.rows = 1;
            shellSettings.extraKeys = {};
            shellSettings.ignoreDoublePresses = false;
            shellSettings.screensaverMinutes = 2;
            closed.clear();
            screen.show();
        }

        function test_listOpensPagesAndBackReturns() {
            verify(control("AppearancePage").activeFocus);
            keyClick(Qt.Key_Down);
            keyClick(Qt.Key_Return);
            compare(control("heading").text, "Accessibility");
            verify(control("text").activeFocus);
            screen.goBack();
            compare(control("heading").text, "Settings");
            // Where it was, not back at the top.
            verify(control("AccessibilityPage").activeFocus);
            compare(closed.count, 0);
            screen.goBack();
            compare(closed.count, 1);
        }

        function test_everyControlIsReachable_data() {
            return [
                {
                    tag: "Appearance",
                    page: 0,
                    names: ["theme", "background", "columns", "rows", "screensaver"]
                },
                {
                    tag: "Accessibility",
                    page: 1,
                    names: ["text", "contrast", "motion", "tiles", "repeat", "double"]
                },
                {
                    tag: "Remotes",
                    page: 2,
                    names: ["back", "home", "menu", "reset"]
                }
            ];
        }

        function test_everyControlIsReachable(data: var) {
            screen.openPage(data.page);
            for (const name of data.names) {
                verify(control(name).activeFocus, name + " isn't focused");
                keyClick(Qt.Key_Down);
            }
        }

        function test_themeIsTriedBeforeItsUsed() {
            screen.openPage(0);
            verify(!control("use").enabled);
            keyClick(Qt.Key_Right);
            compare(shellSettings.theme, "dark");
            verify(control("use").enabled);
            keyClick(Qt.Key_Down);
            verify(control("use").activeFocus);
            keyClick(Qt.Key_Return);
            compare(shellSettings.theme, "high-contrast");
            verify(control("theme").activeFocus);
        }

        function test_previewShowsTheTriedTheme() {
            screen.openPage(0);
            verify(contrast.loadBuiltin("high-contrast"));
            const preview = findChild(screen, "preview");
            verify(!Qt.colorEqual(preview.color, contrast.colors.background));
            keyClick(Qt.Key_Right);
            compare(preview.color, contrast.colors.background);
            compare(preview.Theme.tokens.colors.text, contrast.colors.text);
        }

        function test_accessibility() {
            screen.openPage(1);
            keyClick(Qt.Key_Right);
            compare(shellSettings.textPercent, 110);
            keyClick(Qt.Key_Down);
            keyClick(Qt.Key_Return);
            compare(shellSettings.theme, "high-contrast");
            keyClick(Qt.Key_Down);
            keyClick(Qt.Key_Return);
            compare(shellSettings.reducedMotion, true);
            keyClick(Qt.Key_Down);
            keyClick(Qt.Key_Return);
            compare(shellSettings.columns, 3);
            keyClick(Qt.Key_Return);
            compare(shellSettings.columns, 5);
        }

        function test_remotes() {
            screen.openPage(2);
            keyClick(Qt.Key_Return);
            verify(control("message").text.includes("Back"));
            // Back cancels, and stays on the page.
            screen.goBack();
            compare(control("heading").text, "Remotes");
            screen.goBack();
            compare(control("heading").text, "Settings");
        }

        function test_aButtonMeansOneThing() {
            screen.openPage(2);
            shellSettings.extraKeys = {
                back: ["F5"]
            };
            const remotes = findChild(screen, "remotes");
            remotes.use("home", "F5");
            compare(shellSettings.extraKeys.back, []);
            compare(shellSettings.extraKeys.home, ["F5"]);
            control("reset").forceActiveFocus();
            keyClick(Qt.Key_Return);
            compare(shellSettings.extraKeys.home, []);
        }

        function test_screensaverCanBeOff() {
            screen.openPage(0);
            control("screensaver").forceActiveFocus();
            keyClick(Qt.Key_Left);
            keyClick(Qt.Key_Left);
            compare(shellSettings.screensaverMinutes, 0);
        }

        name: "Settings"
        when: windowShown
    }
}
